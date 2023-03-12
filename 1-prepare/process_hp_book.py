import sys
import re

text = open(sys.argv[1]).read().split('\n')
br = '<|br|>'
eot = '<|endoftext|>'

def setence_ended(line):
  c = line[-1]
  return c == '.' or c == '!' or c == '?' or c == '”'

def filter_page_breaks(lines):
  res = []
  for line in lines:
    page = 'Page |'
    if line.startswith(page):
      continue
    res.append(line)
  return res 

def combine_multi_line_breaks(lines):
  res = []
  for i in range(len(lines)):
    line = lines[i]
    if len(line) == 0:
      if len(res) >= 1:
        if res[-1] != br:
          res.append(br)
      else:
        res.push_back(br)
    else:
      if len(res) >= 1 and res[-1] != br:
        res[-1] += line
      else:
        res.append(line)
  return res

def filter_br(lines):
  res = []
  for line in lines:
    if line != br:
      if line[0].islower() and len(res) >= 1:
        res[-1] += line
      else:
        res.append(line)
  return res

def filter_short_lines(lines):
  res = []
  for line in lines:
    if len(line) < 15:
      continue
    if line.isupper():
      continue
    res.append(line)
  return res

print('\n'.join(filter_br(filter_short_lines(combine_multi_line_breaks(filter_page_breaks(text))))))
