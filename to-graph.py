# coding=utf8

# Prints one callgraph as a "pretty" version, outlining levels visually in
# order of total time percentage

class Node:
    def __init__(self, names, calls, in_main, total_ms, average):
        self.names = names.split(':')
        self.name = self.names[-1]
        self.calls = calls
        self.in_main = in_main
        self.total_ms = total_ms
        self.average = average

import sys
if len(sys.argv) < 2 or not isinstance(sys.argv[1], str):
    print('please provide filename as first argument')
    exit(1)

nodes = []
for line in open(sys.argv[1], 'r'):
    if len(line) <= 1:
        continue

    names = line[: line.find('calls:')].strip()
    line = line[line.find('calls:') + 6:]

    calls = int(line[: line.find('calls in MAIN:')].strip())
    line = line[line.find('calls in MAIN:') + 14:]

    in_main = int(line[: line.find('total:')].strip())
    line = line[line.find('total:') + 6:]

    total_ms = int(line[: line.find('ms')].strip())
    line = line[line.find('ms') + 2:]

    average = int(line[line.find('average:') + 8 : line.find('µs')].strip())
    line = line[line.find('µs') + 2:]

    if names == 'MAIN':
        main = Node(names, calls, in_main, total_ms, average)
    else:
        nodes.append(Node(names, calls, in_main, total_ms, average))

mapped = {}

mapped['calls'] = main.calls
mapped['calls per loop'] = 1
mapped['percentage'] = 100
mapped['average'] = main.average

for node in nodes:
    node_map = mapped
    for name in node.names:
        if name not in node_map:
            node_map[name] = {}
        node_map = node_map[name]

    node_map['calls'] = node.calls
    node_map['calls per loop'] = node.in_main
    node_map['percentage'] = (node.total_ms * 100.0) / main.total_ms
    node_map['average'] = node.average

def pretty(nodes, name = "", level=0):
    if name != "":
        print(('\t' * level) + name + ": " + str(nodes['percentage']) + "%")
    children = [node for node in nodes.iteritems() if isinstance(node[1], dict)]
    for node in sorted(children, key=lambda node: node[1]['percentage'], reverse = True):
        pretty(node[1], node[0], level + 1)

pretty(mapped, level=-1)
