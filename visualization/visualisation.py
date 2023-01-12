"""
ACSE-4 Group 4.2 - Galena
First Created: 2021-03-23

Imperial College London
Department of Earth Science and Engineering

Group members:
    Iñigo Basterretxea Jacob
    Gordon Cheung
    Nina Kahr
    Miguel Pereira
    Ranran Tao
    Suyan Shi
    Jihao Xin
    Jie Zhu
"""

import os
import graphviz
import numpy as np


class Circuit_diagram:
    """
    Class to take in the graphing vector and other information and visualise
    the circuit

    :param genetic_code: the gene vector that describes the layout and flows
            of the circuit
    :type  genetic_code: list or array-like object of integers, length 2n+1,
            for n units in the circuit
    :param file: the absolute path of the circuit data file to visualise
    :type  file: string (optional), default to empty string
    :param gormanium: the gormanium flow volume information of the circuit
    :type  gormanium: list or array-like object of numbers (optional),
                length 2n+1 for n units in the circuit , default to empty list
    :param waste: the waste flow volume information of the circuit
    :type  waste: list or array-like object of numbers (optional),
                length 2n+1 for n units in the circuit , default to empty list
    :param iteration: the generation that produces the circuit
    :type  iteration: int (optional), default to -1
    :param performance: yield of the circuit
    :type  performance: number (optional), default to -np.nan
    :param output_file: file name of the output image file
    :type  output_file: string (optional), default to "gormanium"
    :param g_frac: fraction of the gormanium going to concentration outflow
    :type  g_frac: number (optional), default to 0.2
    :param w_frac: fraction of the waste going to concentration outflow
    :type  w_frac: number (optional), default to 0.05
    :param g_in: the gormanium volume flow into the circuit (kg/s)
    :type  g_in: number (optional), default to 10
    :param w_in: the waste volume flow into the circuit (kg/s)
    :type  w_in: number (optional), default to 100
    """

    def __init__(self, genetic_code=[], file="", gormanium=[], waste=[],
                 iteration=0,
                 performance=-np.nan, output_file="gormanium",
                 g_frac=0.2, w_frac=0.05,
                 g_in=10, w_in=100):

        self.input_file = file
        if file == "":
            # taking in the attributes directly from the constructor
            self.gene = genetic_code
            self.gormanium = gormanium
            self.waste = waste
            self.iteration = iteration
            self.performance = performance
            self.output_file = output_file
        else:
            # read the data from the file
            self.read_data(file)
        self.g_frac = g_frac
        self.w_frac = w_frac
        self.g_in = g_in
        self.w_in = w_in
        self.make_graph()

    """
    Read the relevant data from the specified file
    The file format is specified in "root/data/Note"
    The file should either be 1 line or 3 lines long.
    With the 1 line file only recording the schematic of the circuit, and
    the 4 lines file additionally recording the flows and performance.

    :param file: path of the file to open
    :type  file: string
    """

    def read_data(self, file):
        if os.path.isfile(file):
            # if the specified file can be found
            with open(file) as f:
                print("reading file: ", file)
                # first line should be the circuit schematic
                self.gene = np.fromstring(f.readline(), dtype=int, sep=",")
                # second line should be the inflow data of gormanium
                self.gormanium = np.fromstring(
                    f.readline(), dtype=float, sep=",")
                # third line should be the inflow data of waste
                self.waste = np.fromstring(f.readline(), dtype=float, sep=",")

                if ((len(self.gormanium) > 0) and
                    ((len(self.gormanium) != (len(self.gene)-1)/2 + 2) or
                     (len(self.waste) != (len(self.gene)-1)/2 + 2))):
                    # if flow data is present but mismatch gene data
                    raise ValueError("invalid data file")

                perf = f.readline()
                if perf != "":
                    # third line should be a single float of the performance,
                    # optional
                    self.performance = float(perf)
                else:
                    self.performance = -np.nan
            # extract the other information from the file name
            self.iteration = file.split(".")[-2].split("_")[-1]
            # base the output image file name on the input data file name
            self.output_file = file.split(".")[-2].split(os.sep)[-1]
        else:
            raise ValueError("data file does not exist")

    """
    Make the graph using the parameters read, more details about the
    style and their meanings can be found in "root/visualization.README.md"
    """

    def make_graph(self):
        # define the colour scheme to represent the concentration
        # of gormanium in the flow, from 0% - 100%
        colours = {
            0: "red4",
            1: "red3",
            2: "red2",
            3: "purple3",
            4: "purple2",
            5: "purple1",
            6: "purple",
            7: "mediumslateblue",
            8: "blue2",
            9: "blue3",
            10: "blue4"
        }

        # annotation of the graph
        annot = ""
        if self.iteration != 0:
            annot += "Iteration: " + str(self.iteration) + "\t"
        if self.performance != -np.nan:
            annot += "Performance: " + str(self.performance)

        # graph initialisation
        self.num_nodes = int((len(self.gene)-1)/2)+2
        self.graph = graphviz.Digraph()
        font = "helvectica"
        self.graph.attr(rankdir="LR", splines="spline", strict="true",
                        fontname=font, label=annot)

        # recording the nodes using a dictionary encodes the type of the nodes
        # while still allowing it to be accessed using the index
        self.nodes = dict()
        # find the nodes that represents the key nodes
        self.gene = np.array(self.gene)
        # the tailings element is the largest element in the gene
        self.tailings = max(self.gene)
        # the concentrate element is the second largest element in the gene
        self.concentrate = np.sort(self.gene[self.gene != max(self.gene)])[-1]

        # the feed node
        self.graph.node("Feed", rank="min; Feed",
                        shape="rectangle", style="rounded, filled",
                        fontname=font, colour="black")

        # associate the nodes to its different types
        for i in range(self.num_nodes):
            if i == self.concentrate:
                self.nodes[i] = "Concentrate"
                self.graph.node(self.nodes[i], rank="sink; Concentrate",
                                shape="rectangle", style="rounded, filled",
                                fontname=font, colour="blue")
            elif i == self.tailings:
                self.nodes[i] = "Tailings"
                self.graph.node(self.nodes[i], rank="sink; Tailings",
                                shape="rectangle", style="rounded, filled",
                                fontname=font, colour="red")
            else:
                self.nodes[i] = "Unit " + str(i)
                self.graph.node(self.nodes[i],
                                shape="rectangle",
                                fontname=font, colour="blue")

        # plot the input flow
        if len(self.gormanium) != 0:
            # as the data is in terms of flows into each unit, and the circuit
            # schematic is in terms of flows out from each unit
            # the flow data need to be converted into outflows
            conc_g = np.zeros(len(self.gormanium)-2)
            tail_g = np.zeros(len(self.gormanium)-2)
            conc_w = np.zeros(len(self.waste)-2)
            tail_w = np.zeros(len(self.waste)-2)
            # following get the total flow and ratio of gormanium in them
            # corresponding to the circuit schema
            # first
            flow_vols = np.empty_like(self.gene, dtype=float)
            flow_vols[0] = round(self.g_in + self.w_in, 2)
            # flow ratio, 0=100% -> discretised and rounded to 11 integers 0-10
            flow_ratios = np.empty_like(self.gene)
            flow_ratios[0] = round(self.g_in / (self.g_in + self.w_in), 1)*10

            for i in range(0, len(self.gormanium)-2):
                # first value is just the feed, which can be plot by itself
                # hence start the loop from the second value
                # last two values are inflows to the two sinks
                # which can also be ignored as they are derived through
                # this conversion
                conc_g[i] = round(self.g_frac*self.gormanium[i], 2)
                tail_g[i] = round((1-self.g_frac)*self.gormanium[i], 2)
                conc_w[i] = round(self.w_frac*self.waste[i], 2)
                tail_w[i] = round((1-self.w_frac)*self.waste[i], 2)

                # in the gene array, first value of each cell is the conc
                # outflow, second value is the tailings outflow
                flow_vols[2*i + 1] = round(conc_g[i] + conc_w[i], 2)
                flow_vols[2*i + 2] = round(tail_g[i] + tail_w[i], 2)
                flow_ratios[2*i + 1] = round(conc_g[i] /
                                             (conc_g[i] + conc_w[i]), 1)*10
                flow_ratios[2*i + 2] = round(tail_g[i] /
                                             (tail_g[i] + tail_w[i]), 1)*10

            edge_weights = ((flow_vols) / np.max(flow_vols))*4.4 + 0.6
            self.graph.edge("Feed", self.nodes[self.gene[0]],
                            color=colours[flow_ratios[0]],
                            label="G: " + str(self.g_in)+"kg/s" +
                            "  W: " + str(self.w_in)+"kg/s",
                            penwidth=str(edge_weights[0]), arrowhead="normal",
                            fontname=font, fontsize="11")

        else:
            self.graph.edge("Feed", self.nodes[self.gene[0]], color="black",
                            headport="w", tailport="e",
                            arrowhead="normal")

        for i in range(1, len(self.gene)):
            this_node = int(np.floor((i-1)/2))
            if i % 2 == 1:
                # if odd position -> concentrate
                if len(self.gormanium) != 0:
                    self.graph.edge(self.nodes[this_node],
                                    self.nodes[self.gene[i]],
                                    color=colours[flow_ratios[i]],
                                    label="G: " + str(conc_g[this_node]) +
                                    "kg/s" + "  W: " +
                                    str(conc_w[this_node])+"kg/s",
                                    penwidth=str(edge_weights[i]),
                                    arrowhead="normal",
                                    fontname=font, fontsize="11",
                                    fontcolor="blue")
                else:
                    self.graph.edge(self.nodes[this_node],
                                    self.nodes[self.gene[i]],
                                    color="blue",
                                    arrowhead="normal")
            else:
                # if even position -> tailings
                if len(self.gormanium) != 0:
                    self.graph.edge(self.nodes[this_node],
                                    self.nodes[self.gene[i]],
                                    color=colours[flow_ratios[i]],
                                    style="dashed",
                                    label="G: " + str(tail_g[this_node]) +
                                    "kg/s" + "  W: " +
                                    str(tail_w[this_node])+"kg/s",
                                    penwidth=str(edge_weights[i]),
                                    arrowhead="empty",
                                    fontname=font, fontsize="11",
                                    fontcolor="red")
                else:
                    self.graph.edge(self.nodes[this_node],
                                    self.nodes[self.gene[i]],
                                    color="red", style="dashed",
                                    arrowhead="empty")

    def draw(self):
        path = os.path.join(os.path.dirname(__file__),
                            "figures", self.output_file)
        if self.input_file == "":
            while os.path.isfile(path+".png"):
                # prevent overwriting of files of the same name
                path += "(1)"
        self.graph.render(filename=path,
                          cleanup=True, format="png")


if __name__ == "__main__":
    # if this script is run directly, by default it will visualise
    # all circuit located in the data/ folder, and export it to the
    # visualization/figures folder

    for data in os.listdir(os.path.join(os.path.dirname(__file__),
                                        "..", "data")):
        if (data.endswith(".txt") & data.startswith("circuit")):
            try:
                graph = Circuit_diagram(file=os.path.join(
                    os.path.dirname(__file__), "..", "data", data))
                graph.draw()
            except ValueError:
                continue
