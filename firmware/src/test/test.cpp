#include "test.hpp"

#include <stdio.h>
#include <string.h>

#include <fstream>

#include "ir/Decoder.hpp"

void readWaveform(ir::DecoderState& state, const char* filename) {
  char fullFilename[1000];
  strcpy(fullFilename, __FILE__);
  char* p = strrchr(fullFilename, '/');
  *p = '\0';
  sprintf(fullFilename, "%s/waveforms/%s", fullFilename, filename);

  std::ifstream file(fullFilename);
  if (file.is_open()) {
    std::string line;
    int t_last = 0;
    while (std::getline(file, line)) {
      char cline[1000];
      strcpy(cline, line.c_str());
      if (strlen(cline) > 0) {
        char* d = strchr(cline, ',');
        if (d) {
          *d = '\0';
        }
        int t = atoi(cline);
        state.buffer.push(t - t_last);
        t_last = t;
      }
    }
    file.close();
  }
}

void assert_failed(uint8_t* file, uint32_t line) {
  printf("FAILED: %s: %d\n", file, line);
}