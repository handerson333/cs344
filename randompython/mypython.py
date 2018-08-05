import sys
import string
import os
import csv
import numpy as np
import random

random.seed(a=None)
#make the files
files = ['file1.txt','file2.txt','file3.txt']
#for each file

def letters10(files, currentFile):
	for i in range(0,10):
		temp = random.choice(string.letters).lower()
		with open(files[currentFile],'a') as f:
			f.write(temp)
	print (open(files[currentFile]).read() )



for i in range(0,3):
	#open current file
	file = open(files[i],'w')
	#add 10 random letters to the file
	letters10(files,i)
	


num1 = random.randint(1,42)
print num1
num2 = random.randint(1,42)
print num2
#print files

print num1*num2





