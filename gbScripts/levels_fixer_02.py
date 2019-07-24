#!/usr/bin/python
import sys
import os
import re
import random
import levels_generator
from xml.etree import ElementTree
from re import sub, split, UNICODE
from os import walk, listdir
from os.path import isfile, join, basename
from sets import Set
from array import *

def main(argv):

	level_index = 0

	for argument in argv[:]:

		if "level_index_" in argument:

			level_index = argument[len("level_index_"):]
			print "level index: " + str(level_index)

	if level_index != 0:

		level_name = "../gbBundle/scenes/level_" + str(level_index) + ".tmx"
		print level_name
		data = ElementTree.parse(level_name)
		root = data.getroot()
		buildings_objectgroup_node = {}
		for objectgroup in root:

			if objectgroup.tag == "objectgroup" and objectgroup.attrib["name"] == "buildings":
			
				buildings_objectgroup_node = objectgroup
				for building in objectgroup.findall('object'):

					objectgroup.remove(building)

		map_width = int(root.attrib["width"])
		map_height = int(root.attrib["height"])
		cells_count = map_width * map_height
		cells = []

		cells_node = {}

		for layer in root:

			if layer.tag == "layer" and layer.attrib["name"] == "track":

				cells_node = layer.find("data")


		print cells_node
		for cell in cells_node:

			cells.append(int(cell.attrib["gid"]))

		buildings = []
		levels_generator.generate_buildings(cells, map_width, map_height, buildings)

		index = 1
		for building_it in buildings:
			index = index + 1
			ElementTree.SubElement(buildings_objectgroup_node, "object", id=str(index), x=str(building_it["center"]["x"]), y=str(building_it["center"]["y"]), rotation=str(building_it["rotation"]), name=building_it["name"])

		data.write(level_name);

	else:

		print "wrong level indedx"


if __name__ == "__main__":
    main(sys.argv[1:])
