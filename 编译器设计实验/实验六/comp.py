import os
import sys
args = sys.argv
command = f"../../bin/reference-lexer {args[1]} | ./parser > test.output"
os.system(command)
command = f"../../bin/reference-lexer {args[1]} | ../../bin/reference-parser > standard.output"
os.system(command)
flag = True
with open('test.output', 'r') as f1, open('standard.output', 'r') as f2:
    for line1, line2 in zip(f1, f2):
        if line1 != line2:
            flag = False
            print(f"yourtest: {line1.strip()}")
            print(f"standard: {line2.strip()}")
if flag:
    print("Output is the same as reference-parser. Pass test.")