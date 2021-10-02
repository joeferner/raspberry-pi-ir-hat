use crate::Config;
use std::cell::RefCell;
use std::collections::HashMap;
use std::collections::VecDeque;
use std::fmt;
use std::rc::Rc;
use std::sync::atomic::{AtomicU32, Ordering};

static NEXT_ID: AtomicU32 = AtomicU32::new(0);

pub struct AhoCorasick {
    trie: Rc<RefCell<TrieNode>>,
    cur_node: Rc<RefCell<TrieNode>>,
    tolerance: f32,
}

pub struct TrieNode {
    id: u32,
    signals: Vec<u32>,
    average_signal: u32,
    output: Option<Rc<RefCell<Button>>>,
    failure: Option<Rc<RefCell<TrieNode>>>,
    children: HashMap<u32, Rc<RefCell<TrieNode>>>,
}

impl fmt::Debug for TrieNode {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        let failure = if self.failure.is_some() {
            let failure = self.failure.as_ref().unwrap();
            let failure = failure.borrow();
            format!("{:?}", failure.id)
        } else {
            format!("NONE")
        };
        write!(
            f,
            "node: {{\n(id: {:?}, signals: {:?}, output: {:?}, failure: {:?}): {:?}\n}}",
            self.id, self.signals, self.output, failure, self.children
        )
    }
}

#[derive(Debug)]
pub struct Button {
    remote_name: String,
    button_name: String,
}

impl AhoCorasick {
    pub fn new(config: &Config, tolerance: f32) -> AhoCorasick {
        let root = build_trie(config, tolerance);
        return AhoCorasick {
            trie: root.clone(),
            cur_node: root,
            tolerance,
        };
    }

    pub fn append_find(&mut self, signal: u32) -> Option<Rc<RefCell<Button>>> {
        let child_key = find_child(&self.cur_node.borrow(), signal, self.tolerance);
        match child_key {
            Some(key) => {
                let child_rc = self.cur_node.borrow().children[&key].clone();
                self.cur_node = child_rc.clone();
                let child = child_rc.borrow();
                if child.output.is_some() {
                    let output = child.output.as_ref();
                    return Option::Some(output.unwrap().clone());
                } else {
                    return Option::None;
                }
            }
            None => {
                let has_failure = self.cur_node.borrow().failure.is_some();
                if has_failure {
                    let failure = self.cur_node.borrow().failure.as_ref().unwrap().clone();
                    self.cur_node = failure;
                    return self.append_find(signal);
                } else {
                    self.cur_node = self.trie.clone();
                    return Option::None;
                }
            }
        }
    }
}

fn build_trie(config: &Config, tolerance: f32) -> Rc<RefCell<TrieNode>> {
    let root = Rc::new(RefCell::new(TrieNode {
        id: NEXT_ID.fetch_add(1, Ordering::SeqCst),
        signals: Vec::new(),
        average_signal: 0,
        output: Option::None,
        failure: Option::None,
        children: HashMap::new(),
    }));

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
                &root,
                signals.as_slice(),
                remote_name.to_string(),
                button_name.to_string(),
                tolerance,
            );
        }
    }

    update_failures(&root, tolerance);

    return root;
}

fn add(
    node: &Rc<RefCell<TrieNode>>,
    signals: &[u32],
    remote_name: String,
    button_name: String,
    tolerance: f32,
) {
    if signals.len() == 0 {
        node.borrow_mut().output = Option::Some(Rc::new(RefCell::new(Button {
            remote_name,
            button_name,
        })));
        return;
    }
    let child_key = find_child(&node.borrow(), signals[0], tolerance);
    match child_key {
        Some(c) => {
            let cell = &node.borrow().children[&c];
            {
                let mut cell_mut = cell.borrow_mut();
                cell_mut.signals.push(signals[0]);
                cell_mut.average_signal = average(&cell_mut.signals);
            }
            add(&cell, &signals[1..], remote_name, button_name, tolerance);
        }
        None => {
            let child_signals = vec![signals[0]];
            let c = Rc::new(RefCell::new(TrieNode {
                id: NEXT_ID.fetch_add(1, Ordering::SeqCst),
                signals: child_signals,
                average_signal: signals[0],
                output: Option::None,
                failure: Option::None,
                children: HashMap::new(),
            }));
            add(&c, &signals[1..], remote_name, button_name, tolerance);
            node.borrow_mut().children.insert(signals[0], c);
        }
    }
}

fn find_child(node: &TrieNode, signal: u32, tolerance: f32) -> Option<u32> {
    for key in node.children.keys() {
        let c = node.children[key].borrow();
        let diff =
            f32::abs(((signal as f32) - (c.average_signal as f32)) / (c.average_signal as f32));
        if diff < tolerance {
            return Option::Some(*key);
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

fn update_failures(node_rc: &Rc<RefCell<TrieNode>>, tolerance: f32) {
    let node = node_rc.borrow();

    let mut queue: VecDeque<Rc<RefCell<TrieNode>>> = VecDeque::new();
    for key in node.children.keys() {
        let mut c = node.children[key].borrow_mut();
        c.failure = Option::Some(node_rc.clone());
        queue.push_back(node.children[key].clone());
    }

    while let Some(c) = queue.pop_front() {
        let c_mut = c.borrow();
        for child_key in c_mut.children.keys() {
            let child = &c_mut.children[child_key];
            let failure = Option::Some(find_failure(&child.borrow(), &c, tolerance));
            {
                child.borrow_mut().failure = failure;
            }
            queue.push_back(child.clone());
        }
    }
}

fn find_failure(
    node: &TrieNode,
    parent: &Rc<RefCell<TrieNode>>,
    tolerance: f32,
) -> Rc<RefCell<TrieNode>> {
    let mut failure: Rc<RefCell<TrieNode>> = parent.clone();
    loop {
        let failure_clone = failure.clone();
        let mut failure_node = failure_clone.borrow();
        let mut matching_child_key = find_child(&mut failure_node, node.signals[0], tolerance);
        if matching_child_key.is_some()
            && failure_node.children[&matching_child_key.unwrap()]
                .borrow()
                .id
                == node.id
        {
            matching_child_key = Option::None;
        }
        match matching_child_key {
            Some(matching_child_key) => {
                return failure_node.children[&matching_child_key].clone();
            }
            None => match &failure_node.failure {
                Some(f) => {
                    failure = f.clone();
                }
                None => {
                    return failure.clone();
                }
            },
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::ConfigButton;
    use crate::ConfigRemote;

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
        assert_eq!(r.borrow().remote_name, "remote1");
        assert_eq!(r.borrow().button_name, "button3");
    }
}
