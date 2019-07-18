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
		for objectgroup in root:

			if objectgroup.tag == "objectgroup" and objectgroup.attrib["name"] == "buildings":
			
				for building in objectgroup.iter('object'):

					for buiding_index, buiding_name in enumerate(levels_generator.k_office_buildings_2x2_ground_floor):

						if building.attrib["name"] == buiding_name:

							position_x = float(building.attrib["x"])
							position_y = float(building.attrib["y"])
							position_x -= 128.0
							position_y += 128.0;

							building.set('x', str(position_x))
							building.set('y', str(position_y))

							building.set('gid', str(135))
							building.set('width', str(256))
							building.set('height', str(256))


					for buiding_index, buiding_name in enumerate(levels_generator.k_appartment_01_buildings_1x2_ground_floor):

						if building.attrib["name"] == buiding_name:

							position_x = float(building.attrib["x"])
							position_y = float(building.attrib["y"])
							rotation = 0

							if building.get("rotation"):
								rotation = float(building.attrib["rotation"])

							if rotation == 0.0:

 								position_x -= 64.0
								position_y += 128.0;

							elif rotation == 90:

								position_x -= 128.0
								position_y -= 64.0;


							elif rotation == 180:

								position_x += 64.0
								position_y -= 128.0;

							elif rotation == 270:

								position_x += 128.0
								position_y += 64.0;


							building.set('x', str(position_x))
							building.set('y', str(position_y))

							building.set('gid', str(136))
							building.set('width', str(128))
							building.set('height', str(256))


					for buiding_index, buiding_name in enumerate(levels_generator.k_office_buildings_1x2_ground_floor):

						if building.attrib["name"] == buiding_name:

							position_x = float(building.attrib["x"])
							position_y = float(building.attrib["y"])
							rotation = 0

							if building.get("rotation"):
								rotation = float(building.attrib["rotation"])

							if rotation == 0.0:

 								position_x -= 64.0
								position_y += 128.0;

							elif rotation == 90:

								position_x -= 128.0
								position_y -= 64.0;


							elif rotation == 180:

								position_x += 64.0
								position_y -= 128.0;

							elif rotation == 270:

								position_x += 128.0
								position_y += 64.0;


							building.set('x', str(position_x))
							building.set('y', str(position_y))

							building.set('gid', str(137))
							building.set('width', str(128))
							building.set('height', str(256))


		data.write(level_name);

	else:

		print "wrong level indedx"


if __name__ == "__main__":
    main(sys.argv[1:])
