#!/usr/bin/env python

import os
import subprocess

class RandomFileTest:
    def __init__(self, fname):
        self.fname = fname
        self.ntest = 0
        self.nsuccess = 0

    def test_filepdf(self, size_f, offset_f):
        self.ntest = self.ntest + 1
        p = subprocess.Popen(["bazel-bin/tests/file_generator", self.fname, str(size_f), str(offset_f)])
        out, err = p.communicate()
        if p.returncode != 0:
            print("Error with file_generator")
            return
        p = subprocess.Popen(["bazel-bin/src/pdf_end", self.fname], stdout=subprocess.PIPE)
        out, err = p.communicate()
        outs = out.split()
        os.remove(self.fname)
        if int(outs[0]) != offset_f:
            print(f"Error with {size_f}:{offset_f} given: {outs[0]}")
            return
        self.nsuccess = self.nsuccess + 1
    
    def print(self):
        print(f"Number of success: {self.nsuccess}/{self.ntest}")

rd_test = RandomFileTest("test_file")
# Empty file
nb_test = rd_test.test_filepdf(0, -1)

# Diverse sizes of files
nb_test = rd_test.test_filepdf(65532, 16025)
nb_test = rd_test.test_filepdf(65532, 16025)
nb_test = rd_test.test_filepdf(8198, 8188)
nb_test = rd_test.test_filepdf(8198, 4093)
nb_test = rd_test.test_filepdf(8193, 4094)
nb_test = rd_test.test_filepdf(4096, 4089)
nb_test = rd_test.test_filepdf(512, 504)
rd_test.print()

