import graphviz

# start with a graphviz object
graph = graphviz.Digraph()

graph.attr(rankdir='LR')
graph.attr('node', shape='rectangle')

# Create the graph. New nodes are created automatically
# when first named.

graph.edge('Feed', 'Unit 1', color='blue',
           headport='w', tailport='e',
           arrowhead='normal', arrowtail='normal')

graph.edge('Unit 1', 'Unit 2', color='blue',
           headport='w', tailport='n',
           arrowhead='normal')
graph.edge(
    'Unit 1', 'Unit 3', color='red', headport='w', tailport='s',
    arrowhead='normal')

graph.edge('Unit 2', 'Concentrate', color='blue',
           headport='w', tailport='n',
           arrowhead='normal')
graph.edge(
    'Unit 2', 'Unit 1', color='red', headport='w', tailport='s',
    arrowhead='normal')

graph.edge('Unit 3', 'Unit 1', color='blue',
           headport='w', tailport='n',
           arrowhead='normal')
graph.edge(
    'Unit 3', 'Tailings', color='red', headport='w', tailport='s',
    arrowhead='normal')

# Write to disk

graph.render(filename='example', cleanup=True, format='png')
