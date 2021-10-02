use crate::Config;
use std::collections::VecDeque;
use std::fmt;

static ROOT_ID: usize = 0;
static NOT_SET_ID: usize = usize::MAX;

pub struct AhoCorasick {
    nodes: Vec<TrieNode>,
    cur_node: usize,
    tolerance: f32,
}

pub struct TrieNode {
    id: usize,
    signals: Vec<u32>,
    average_signal: u32,
    output: Option<Button>,
    failure: usize,
    children: Vec<usize>,
}

impl fmt::Debug for TrieNode {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(
            f,
            "node: {{\n(id: {:?}, signals: {:?}, output: {:?}, failure: {:?}):\n  {:?}\n}}",
            self.id, self.signals, self.output, self.failure, self.children
        )
    }
}

#[derive(Debug)]
pub struct Button {
    pub remote_name: String,
    pub button_name: String,
}

impl AhoCorasick {
    pub fn new(config: &Config, tolerance: f32) -> AhoCorasick {
        return AhoCorasick {
            nodes: build_trie(config, tolerance),
            cur_node: ROOT_ID,
            tolerance,
        };
    }

    pub fn append_find(&mut self, signal: u32) -> Option<&Button> {
        let child_index = find_child(&self.nodes, self.cur_node, signal, self.tolerance);
        match child_index {
            Some(child_index) => {
                self.cur_node = child_index;
                match &self.nodes[child_index].output {
                    Some(output) => {
                        return Option::Some(output);
                    }
                    None => {
                        return Option::None;
                    }
                }
            }
            None => {
                if self.nodes[self.cur_node].failure == self.cur_node {
                    return Option::None;
                }
                self.cur_node = self.nodes[self.cur_node].failure;
                return self.append_find(signal);
            }
        }
    }
}

fn build_trie(config: &Config, tolerance: f32) -> Vec<TrieNode> {
    let mut nodes = Vec::new();
    let root = TrieNode {
        id: ROOT_ID,
        signals: Vec::new(),
        average_signal: 0,
        output: Option::None,
        failure: ROOT_ID,
        children: Vec::new(),
    };
    nodes.push(root);

    for remote_name in config.remotes.keys() {
        let remote = config.remotes.get(remote_name).unwrap();
        for button_name in remote.buttons.keys() {
            let button = remote.buttons.get(button_name).unwrap();
            let signals: Vec<u32> = button
                .signal
                .split(",")
                .map(|s| s.trim().parse::<u32>().unwrap())
                .collect();
            add(
                &mut nodes,
                ROOT_ID,
                signals.as_slice(),
                remote_name.to_string(),
                button_name.to_string(),
                tolerance,
            );
        }
    }

    update_failures(&mut nodes, tolerance);

    return nodes;
}

fn add(
    nodes: &mut Vec<TrieNode>,
    node_index: usize,
    signals: &[u32],
    remote_name: String,
    button_name: String,
    tolerance: f32,
) {
    if signals.len() == 0 {
        nodes[node_index].output = Option::Some(Button {
            remote_name,
            button_name,
        });
        return;
    }
    let child_index = find_child(&nodes, node_index, signals[0], tolerance);
    match child_index {
        Some(child_index) => {
            nodes[child_index].signals.push(signals[0]);
            nodes[child_index].average_signal = average(&nodes[child_index].signals);
            add(
                nodes,
                child_index,
                &signals[1..],
                remote_name,
                button_name,
                tolerance,
            );
        }
        None => {
            let new_node_id = nodes.len();
            let new_node = TrieNode {
                id: new_node_id,
                signals: vec![signals[0]],
                average_signal: signals[0],
                output: Option::None,
                failure: NOT_SET_ID,
                children: Vec::new(),
            };
            nodes.push(new_node);
            nodes[node_index].children.push(new_node_id);
            add(
                nodes,
                new_node_id,
                &signals[1..],
                remote_name,
                button_name,
                tolerance,
            );
        }
    }
}

fn find_child(
    nodes: &Vec<TrieNode>,
    node_index: usize,
    signal: u32,
    tolerance: f32,
) -> Option<usize> {
    for child_index in &nodes[node_index].children {
        let child = &nodes[*child_index];
        let diff = f32::abs(
            ((signal as f32) - (child.average_signal as f32)) / (child.average_signal as f32),
        );
        if diff < tolerance {
            return Option::Some(*child_index);
        }
    }
    return Option::None;
}

fn average(signals: &Vec<u32>) -> u32 {
    let mut sum = 0;
    for s in signals {
        sum += s;
    }
    return sum / signals.len() as u32;
}

fn update_failures(nodes: &mut Vec<TrieNode>, tolerance: f32) {
    let mut queue: VecDeque<usize> = VecDeque::new();
    for child_index in nodes[ROOT_ID].children.to_vec() {
        nodes[child_index].failure = ROOT_ID;
        queue.push_back(child_index);
    }

    while let Some(node_index) = queue.pop_front() {
        for child_index in &mut nodes[node_index].children.to_vec() {
            nodes[node_index].failure = find_failure(nodes, *child_index, node_index, tolerance);
            queue.push_back(*child_index);
        }
    }
}

fn find_failure(
    nodes: &Vec<TrieNode>,
    node_index: usize,
    parent_index: usize,
    tolerance: f32,
) -> usize {
    let mut failure_index = parent_index;
    loop {
        let mut matching_child_index = find_child(
            nodes,
            failure_index,
            nodes[node_index].signals[0],
            tolerance,
        );
        if matching_child_index.is_some()
            && nodes[matching_child_index.unwrap()].id == nodes[node_index].id
        {
            matching_child_index = Option::None;
        }
        match matching_child_index {
            Some(matching_child_index) => {
                return matching_child_index;
            }
            None => {
                if failure_index != NOT_SET_ID {
                    return failure_index;
                }
                failure_index = nodes[failure_index].failure;
            }
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::ConfigButton;
    use crate::ConfigRemote;
    use std::collections::HashMap;

    #[test]
    fn test_empty() {
        let config = Config {
            remotes: HashMap::new(),
        };
        let mut aho = AhoCorasick::new(&config, 0.1);
        assert!(aho.append_find(100).is_none());
        assert!(aho.append_find(200).is_none());
    }

    #[test]
    fn test_multiple_buttons() {
        let mut remote1_buttons: HashMap<String, ConfigButton> = HashMap::new();
        remote1_buttons.insert(
            "button1".to_string(),
            ConfigButton {
                debounce: Option::None,
                signal: "100,200,300".to_string(),
            },
        );
        remote1_buttons.insert(
            "button2".to_string(),
            ConfigButton {
                debounce: Option::None,
                signal: "100,300,300".to_string(),
            },
        );
        remote1_buttons.insert(
            "button3".to_string(),
            ConfigButton {
                debounce: Option::None,
                signal: "200,500,600,700".to_string(),
            },
        );

        let remote1 = ConfigRemote {
            buttons: remote1_buttons,
        };

        let mut remotes: HashMap<String, ConfigRemote> = HashMap::new();
        remotes.insert("remote1".to_string(), remote1);

        let config = Config { remotes };
        let mut aho = AhoCorasick::new(&config, 0.1);

        // no match
        assert!(aho.append_find(200).is_none());
        // partial match
        assert!(aho.append_find(100).is_none());
        assert!(aho.append_find(200).is_none());
        assert!(aho.append_find(500).is_none());
        // match
        assert!(aho.append_find(600).is_none());
        let r = aho.append_find(700);
        assert!(r.is_some());
        let r = r.unwrap();
        assert_eq!(r.remote_name, "remote1");
        assert_eq!(r.button_name, "button3");
    }
}
