#!/usr/bin/python
import sys
import os
import re
import random
from xml.etree import ElementTree
from re import sub, split, UNICODE
from os import walk, listdir
from os.path import isfile, join, basename
from sets import Set
from array import *

k_min_map_size = 16
k_max_map_size = 24

k_tile_size = 128

k_empty_tile_id = 22
k_straight_road_up_down_id = 1
k_straight_road_left_right_id = 2
k_corner_road_down_right = 8
k_corner_road_left_down = 9
k_corner_road_up_left = 16
k_corner_road_right_up = 15

k_offset_from_border = 4
k_wall_offset = 16

k_straight_road_up_down_points_0 = "16,-64 16,64"
k_straight_road_up_down_points_1 = "112,-64 112,64"
k_straight_road_left_right_points_0 = "0,-64 128,-64"
k_straight_road_left_right_points_1 = "0,64 128,64"
k_corner_road_down_right_points_0 = "16,0 32,-64 64,-96 128,-112"
k_corner_road_down_right_points_1 = "112,0 114,-8 120,-14 128,-16"
k_corner_road_left_down_points_0 = "0,-112 64,-96 96,-64 112,0"
k_corner_road_left_down_points_1 = "0,-16 8,-14 14,-8 16,0"
k_corner_road_up_left_points_0 = "0,-112 64,-96 96,-64 112,0"
k_corner_road_up_left_points_1 = "0,-16 8,-14 14,-8 16,0"
k_corner_road_right_up_points_0 = "0,-112 64,-96 96,-64 112,0"
k_corner_road_right_up_points_1 = "0,-16 8,-14 14,-8 16,0"

def add_straight_road_up_down_walls(walls, x, y):
	wall = {}
	wall["x"] = x * k_tile_size
	wall["y"] = y * k_tile_size + k_tile_size * 0.5 + k_wall_offset
	wall["points"] = k_straight_road_left_right_points_0
	walls.append(wall)
	wall = {}
	wall["x"] = x * k_tile_size
	wall["y"] = y * k_tile_size + k_tile_size * 0.5 - k_wall_offset
	wall["points"] = k_straight_road_left_right_points_1
	walls.append(wall)

def add_straight_road_left_right_walls(walls, x, y):
	wall = {}
	wall["x"] = x * k_tile_size
	wall["y"] = y * k_tile_size + k_tile_size * 0.5
	wall["points"] = k_straight_road_up_down_points_0
	walls.append(wall)
	wall = {}
	wall["x"] = x * k_tile_size
	wall["y"] = y * k_tile_size + k_tile_size * 0.5
	wall["points"] = k_straight_road_up_down_points_1
	walls.append(wall)

def add_corner_road_down_right_walls(walls, x, y):
	wall = {}
	wall["x"] = x * k_tile_size
	wall["y"] = y * k_tile_size + k_tile_size
	wall["points"] = k_corner_road_down_right_points_0
	walls.append(wall)
	wall = {}
	wall["x"] = x * k_tile_size
	wall["y"] = y * k_tile_size + k_tile_size
	wall["points"] = k_corner_road_down_right_points_1
	walls.append(wall)

def add_corner_road_left_down_walls(walls, x, y):
	wall = {}
	wall["x"] = x * k_tile_size
	wall["y"] = y * k_tile_size + k_tile_size
	wall["points"] = k_corner_road_left_down_points_0
	walls.append(wall)
	wall = {}
	wall["x"] = x * k_tile_size
	wall["y"] = y * k_tile_size + k_tile_size
	wall["points"] = k_corner_road_left_down_points_1
	walls.append(wall)

def add_corner_road_up_left_walls(walls, x, y):
	wall = {}
	wall["x"] = x * k_tile_size
	wall["y"] = y * k_tile_size + k_tile_size
	wall["points"] = k_corner_road_up_left_points_0
	walls.append(wall)
	wall = {}
	wall["x"] = x * k_tile_size
	wall["y"] = y * k_tile_size + k_tile_size
	wall["points"] = k_corner_road_up_left_points_1
	walls.append(wall)

def add_corner_road_right_up_walls(walls, x, y):
	wall = {}
	wall["x"] = x * k_tile_size
	wall["y"] = y * k_tile_size + k_tile_size
	wall["points"] = k_corner_road_right_up_points_0
	walls.append(wall)
	wall = {}
	wall["x"] = x * k_tile_size
	wall["y"] = y * k_tile_size + k_tile_size
	wall["points"] = k_corner_road_right_up_points_1
	walls.append(wall)


def find_route(path, current_point, road, map_width, map_height):
	x = current_point["x"]
	y = current_point["y"]

	for road_it in road:

		if road_it["x"] == x - 1 and road_it["y"] == y and road_it["marked_iteration"] == 0:
							
			path.append(road_it)
			road_it["marked_iteration"] = 1
			find_route(path, road_it, road, map_width, map_height)

		if road_it["x"] == x + 1 and road_it["y"] == y and road_it["marked_iteration"] == 0:
							
			path.append(road_it)
			road_it["marked_iteration"] = 1
			find_route(path, road_it, road, map_width, map_height)

		if road_it["x"] == x and road_it["y"] == y - 1 and road_it["marked_iteration"] == 0:
							
			path.append(road_it)
			road_it["marked_iteration"] = 1
			find_route(path, road_it, road, map_width, map_height)

		if road_it["x"] == x and road_it["y"] == y + 1 and road_it["marked_iteration"] == 0:
							
			path.append(road_it)
			road_it["marked_iteration"] = 1
			find_route(path, road_it, road, map_width, map_height)
							

def generate_clockwise_oriented_route(route_data, oriented_route_data):

	route_length = len(route_data)
	leftmost_point_index = 0;
	for i in range(0, route_length):

		if int(route_data[i]["x"]) < int(route_data[leftmost_point_index]["x"]):
			leftmost_point_index = i


	start_point_index = leftmost_point_index;
	
	while 1:

		end_point_index = (start_point_index + 1) % route_length
		for i in range(0, route_length):

			point_01_x = int(route_data[start_point_index]["x"])
			point_01_y = int(route_data[start_point_index]["y"])

			point_02_x = int(route_data[i]["x"])
			point_02_y = int(route_data[i]["y"])

			point_03_x = int(route_data[end_point_index]["x"])
			point_03_y = int(route_data[end_point_index]["y"])

			result = (point_02_y - point_01_y) * (point_03_x - point_02_x) - (point_02_x - point_01_x) * (point_03_y - point_02_y)
			if result < 0:
				end_point_index = i


		oriented_route_data.append(route_data[end_point_index])
		start_point_index = end_point_index

		if start_point_index == leftmost_point_index:
			break

def generate_next_corner(initial_direction_id, next_location_x, next_location_y, map_data, map_width, map_height, additional_route_data, iteration):
	
	direction_x = 0
	direction_y = 0
	next_direction_id = 0
	next_route_node = {"x": 0, "y": 0, "id": 0}

	if initial_direction_id == k_corner_road_left_down:

		if iteration == 0:

			map_data[next_location_x + next_location_y * map_width] = k_corner_road_right_up
			next_route_node = {"x": next_location_x, "y": next_location_y, "id": k_corner_road_right_up}
			additional_route_data.append(next_route_node)
			iteration = 1
			direction_x = 1
			direction_y = 0
			next_direction_id = k_straight_road_left_right_id

		elif iteration == 1:

			map_data[next_location_x + next_location_y * map_width] = k_corner_road_up_left
			next_route_node = {"x": next_location_x, "y": next_location_y, "id": k_corner_road_up_left}
			additional_route_data.append(next_route_node)
			iteration = 2
			direction_x = 0
			direction_y = -1
			next_direction_id = k_straight_road_up_down_id

		elif iteration == 2:

			map_data[next_location_x + next_location_y * map_width] = k_corner_road_down_right
			next_route_node = {"x": next_location_x, "y": next_location_y, "id": k_corner_road_down_right}
			additional_route_data.append(next_route_node)

	elif initial_direction_id == k_corner_road_right_up:

		if iteration == 0:

			map_data[next_location_x + next_location_y * map_width] = k_corner_road_left_down
			next_route_node = {"x": next_location_x, "y": next_location_y, "id": k_corner_road_left_down}
			additional_route_data.append(next_route_node)
			iteration = 1
			direction_x = -1
			direction_y = 0
			next_direction_id = k_straight_road_left_right_id

		elif iteration == 1:

			map_data[next_location_x + next_location_y * map_width] = k_corner_road_down_right
			next_route_node = {"x": next_location_x, "y": next_location_y, "id": k_corner_road_down_right}
			additional_route_data.append(next_route_node)
			iteration = 2
			direction_x = 0
			direction_y = 1
			next_direction_id = k_straight_road_up_down_id

		elif iteration == 2:

			map_data[next_location_x + next_location_y * map_width] = k_corner_road_up_left
			next_route_node = {"x": next_location_x, "y": next_location_y, "id": k_corner_road_up_left}
			additional_route_data.append(next_route_node)

	elif initial_direction_id == k_corner_road_up_left:

		if iteration == 0:

			map_data[next_location_x + next_location_y * map_width] = k_corner_road_down_right
			next_route_node = {"x": next_location_x, "y": next_location_y, "id": k_corner_road_down_right}
			additional_route_data.append(next_route_node)
			iteration = 1
			direction_x = 0
			direction_y = 1
			next_direction_id = k_straight_road_up_down_id

		elif iteration == 1:

			map_data[next_location_x + next_location_y * map_width] = k_corner_road_right_up
			next_route_node = {"x": next_location_x, "y": next_location_y, "id": k_corner_road_right_up}
			additional_route_data.append(next_route_node)
			iteration = 2
			direction_x = 1
			direction_y = 0
			next_direction_id = k_straight_road_left_right_id

		elif iteration == 2:

			map_data[next_location_x + next_location_y * map_width] = k_corner_road_left_down
			next_route_node = {"x": next_location_x, "y": next_location_y, "id": k_corner_road_left_down}
			additional_route_data.append(next_route_node)

	elif initial_direction_id == k_corner_road_down_right:

		if iteration == 0:

			map_data[next_location_x + next_location_y * map_width] = k_corner_road_up_left
			next_route_node = {"x": next_location_x, "y": next_location_y, "id": k_corner_road_up_left}
			additional_route_data.append(next_route_node)
			iteration = 1
			direction_x = 0
			direction_y = -1
			next_direction_id = k_straight_road_up_down_id

		elif iteration == 1:

			map_data[next_location_x + next_location_y * map_width] = k_corner_road_left_down
			next_route_node = {"x": next_location_x, "y": next_location_y, "id": k_corner_road_left_down}
			additional_route_data.append(next_route_node)
			iteration = 2
			direction_x = -1
			direction_y = 0
			next_direction_id = k_straight_road_left_right_id

		elif iteration == 2:

			map_data[next_location_x + next_location_y * map_width] = k_corner_road_right_up
			next_route_node = {"x": next_location_x, "y": next_location_y, "id": k_corner_road_right_up}
			additional_route_data.append(next_route_node)


	return (iteration, direction_x, direction_y, next_direction_id, next_route_node)


def generate_curve(initial_direction_id, initial_x, initial_y, next_direction_id, direction_x, direction_y, map_data, route_data, route_it, additional_route_data, map_width, map_height, iteration):

	#print "generate curve:"
	#print "initial x: " + str(initial_x)
	#print "initial y: " + str(initial_y)
	#print "next direction id: " + str(next_direction_id)
	#print "direction x: " + str(direction_x)
	#print "direction y: " + str(direction_y)
	is_generated = 1
	end_curve_route_node_x = 0
	end_curve_route_node_y = 0
	next_route_node_x = route_it["x"] + direction_x
	next_route_node_y = route_it["y"] + direction_y
	if next_route_node_x > k_offset_from_border and next_route_node_x < ((map_width - 1) - k_offset_from_border) and next_route_node_y > k_offset_from_border and next_route_node_y < ((map_height - 1) - k_offset_from_border):
		
		next_map_data_it = map_data[next_route_node_x + next_route_node_y * map_width]
		if next_map_data_it == k_empty_tile_id:

			if iteration < 2:

				next_next_route_node_x = route_it["x"] + direction_x * 2
				next_next_route_node_y = route_it["y"] + direction_y * 2
				if next_next_route_node_x > k_offset_from_border and next_next_route_node_x < ((map_width - 1) - k_offset_from_border) and next_next_route_node_y > k_offset_from_border and next_next_route_node_y < ((map_height - 1) - k_offset_from_border):

					next_next_map_data_it = map_data[next_next_route_node_x + next_next_route_node_y * map_width]
					if next_next_map_data_it == k_empty_tile_id:

						map_data[next_route_node_x + next_route_node_y * map_width] = next_direction_id
						next_route_node = {"x": next_route_node_x, "y": next_route_node_y, "id": next_direction_id}
						additional_route_data.append(next_route_node)

						should_generate_corner = random.random() > 0.75 and iteration < 2
						if should_generate_corner:

							result = generate_next_corner(initial_direction_id, next_route_node_x, next_route_node_y, map_data, map_width, map_height, additional_route_data, iteration)
							iteration = result[0]
							direction_x = result[1]
							direction_y = result[2]
							next_direction_id = result[3]
							next_route_node = result[4]

						result = generate_curve(initial_direction_id, initial_x, initial_y, next_direction_id, direction_x, direction_y, map_data, route_data, next_route_node, additional_route_data, map_width, map_height, iteration)
						end_curve_route_node_x = result[1]
						end_curve_route_node_y = result[2]
					
					else:

						result = generate_next_corner(initial_direction_id, next_route_node_x, next_route_node_y, map_data, map_width, map_height, additional_route_data, iteration)
						iteration = result[0]
						direction_x = result[1]
						direction_y = result[2]
						next_direction_id = result[3]
						next_route_node = result[4]

						result = generate_curve(initial_direction_id, initial_x, initial_y, next_direction_id, direction_x, direction_y, map_data, route_data, next_route_node, additional_route_data, map_width, map_height, iteration)
						end_curve_route_node_x = result[1]
						end_curve_route_node_y = result[2]
				
				else:

					result = generate_next_corner(initial_direction_id, next_route_node_x, next_route_node_y, map_data, map_width, map_height, additional_route_data, iteration)
					iteration = result[0]
					direction_x = result[1]
					direction_y = result[2]
					next_direction_id = result[3]
					next_route_node = result[4]

					result = generate_curve(initial_direction_id, initial_x, initial_y, next_direction_id, direction_x, direction_y, map_data, route_data, next_route_node, additional_route_data, map_width, map_height, iteration)
					end_curve_route_node_x = result[1]
					end_curve_route_node_y = result[2]
			
			else:

				map_data[next_route_node_x + next_route_node_y * map_width] = next_direction_id
				next_route_node = {"x": next_route_node_x, "y": next_route_node_y, "id": next_direction_id}
				additional_route_data.append(next_route_node)
				result = generate_curve(initial_direction_id, initial_x, initial_y, next_direction_id, direction_x, direction_y, map_data, route_data, next_route_node, additional_route_data, map_width, map_height, iteration)
				end_curve_route_node_x = result[1]
				end_curve_route_node_y = result[2]
		
		else:

			if iteration == 2:

				generate_next_corner(initial_direction_id, next_route_node_x, next_route_node_y, map_data, map_width, map_height, additional_route_data, iteration)
				end_curve_route_node_x = next_route_node_x
				end_curve_route_node_y = next_route_node_y

			else:

				is_generated = 0

	else:

		if iteration == 2:

			generate_next_corner(initial_direction_id, next_route_node_x, next_route_node_y, map_data, map_width, map_height, additional_route_data, iteration)
			end_curve_route_node_x = next_route_node_x
			end_curve_route_node_y = next_route_node_y

		else:

			is_generated = 0
	
	return (is_generated, end_curve_route_node_x, end_curve_route_node_y)



def main(argv):

	for argument in argv[:]:

		print argument

	map_width = random.randrange(k_min_map_size, k_max_map_size, 1)
	map_height = random.randrange(k_min_map_size, k_max_map_size, 1)
	tiles_count = map_width * map_height
	curves_num = 7

	tiles = []
	walls = []

	road = [];

	for i in range(int(tiles_count)):
		tiles.append(k_empty_tile_id)

	for i in range(int(map_width)):

		for j in range(int(map_height)):

			tile_id = k_empty_tile_id;
			if i > k_offset_from_border and i < ((map_width - 1) - k_offset_from_border) and j == k_offset_from_border:

				tile_id = k_straight_road_left_right_id
				add_straight_road_up_down_walls(walls, i, j)

			if i > k_offset_from_border and i < ((map_width - 1) - k_offset_from_border) and j == ((map_height - 1) - k_offset_from_border):

				tile_id = k_straight_road_left_right_id
				add_straight_road_up_down_walls(walls, i, j)

			if j > k_offset_from_border and j < ((map_height - 1) - k_offset_from_border) and i == k_offset_from_border:

				tile_id = k_straight_road_up_down_id
				add_straight_road_left_right_walls(walls, i, j)

			if j > k_offset_from_border and j < ((map_height - 1) - k_offset_from_border) and i == ((map_width - 1) - k_offset_from_border):

				tile_id = k_straight_road_up_down_id
				add_straight_road_left_right_walls(walls, i, j)

			if i == k_offset_from_border and j == k_offset_from_border:

				tile_id = k_corner_road_down_right
				add_corner_road_down_right_walls(walls, i, j)

			if i == k_offset_from_border and j == ((map_height - 1) - k_offset_from_border):

				tile_id = k_corner_road_right_up
				add_corner_road_right_up_walls(walls, i, j)

			if i == ((map_width - 1) - k_offset_from_border) and j == k_offset_from_border:
				
				tile_id = k_corner_road_left_down
				add_corner_road_left_down_walls(walls, i, j)

			if i == ((map_width - 1) - k_offset_from_border) and j == ((map_height - 1) - k_offset_from_border):
				
				tile_id = k_corner_road_up_left
				add_corner_road_up_left_walls(walls, i, j)

			tiles[i + j * map_width] = tile_id
			if tile_id != k_empty_tile_id:
				road.append({"x":i,"y":j,"id": tile_id, "marked_iteration": 0})

	leftmost_location = road[0]
	for road_it in road:

		if road_it["x"] <= leftmost_location["x"] and road_it["y"] <= leftmost_location["y"]:
			leftmost_location = road_it
			#start_position = road_it;
			#break

	print "leftmost location: " + str(leftmost_location)

	next_after_leftmost_location = leftmost_location
	for road_it in road:

		if road_it["x"] == leftmost_location["x"] + 1:
			
			next_after_leftmost_location = road_it
			break

	print "next after leftmost location: " + str(next_after_leftmost_location)
	#for road_it in road:
	#	if road_it["id"] == k_straight_road_left_right_id or road_it["id"] == k_straight_road_up_down_id:

	#		is_start_position = random.random() > 0.95
	#		if is_start_position:

	#			start_position = road_it
	#			break

	route_data = []
	route_data.append(leftmost_location)
	leftmost_location["marked_iteration"] = 1

	route_data.append(next_after_leftmost_location)
	next_after_leftmost_location["marked_iteration"] = 1


	find_route(route_data, next_after_leftmost_location, road, map_width, map_height)
	#oriented_route_data = []
	#generate_clockwise_oriented_route(road, oriented_route_data)
	#print "route length: " + str(len(route_data))
	#print "oriented route length: " + str(len(oriented_route_data))
	#route_data = oriented_route_data

	#print "route length: " + str(len(route_data))
	#print route_data

	route_length = len(route_data)
	curves_interval = int(route_length / curves_num)
	curve_start_node_index = curves_interval
	curves_iterations_num = 0

	while 1:

		result = (0, 0, 0)
		curve_start_node = route_data[curve_start_node_index]
		if curve_start_node["id"] == k_straight_road_left_right_id or curve_start_node["id"] == k_straight_road_up_down_id:

			curve_start_node_x = curve_start_node["x"]
			curve_start_node_y = curve_start_node["y"]

			next_route_node = route_data[curve_start_node_index + 1]
			if next_route_node["id"] == curve_start_node["id"]:

				next_route_node_x = next_route_node["x"]
				next_route_node_y = next_route_node["y"]

				delta_x = next_route_node_x - curve_start_node_x
				delta_y = next_route_node_y - curve_start_node_y

				current_direction_id = curve_start_node["id"]
				next_direction_id = current_direction_id
				direction_x = 0
				direction_y = 0

				if delta_x > 0:

					current_direction_id = k_corner_road_left_down
					next_direction_id = k_straight_road_up_down_id
					direction_y = 1

				elif delta_x < 0:

					current_direction_id = k_corner_road_right_up
					next_direction_id = k_straight_road_up_down_id
					direction_y = -1

				elif delta_y > 0:

					current_direction_id = k_corner_road_up_left
					next_direction_id = k_straight_road_left_right_id
					direction_x = -1

				elif delta_y < 0:

					current_direction_id = k_corner_road_down_right
					next_direction_id = k_straight_road_left_right_id
					direction_x = 1

				old_direction_id = tiles[curve_start_node_x + curve_start_node_y * map_width]
				tiles[curve_start_node_x + curve_start_node_y * map_width] = current_direction_id
				curve_start_node["id"] = current_direction_id
				additional_route_data = []
				result = generate_curve(current_direction_id, curve_start_node_x, curve_start_node_y, next_direction_id, direction_x, direction_y, tiles, route_data, curve_start_node, additional_route_data, map_width, map_height, 0)
				if result[0] == 0:
					tiles[curve_start_node_x + curve_start_node_y * map_width] = old_direction_id


		if result[0] != 0:

			print "curve end node x: " + str(result[1])
			print "curve end node y: " + str(result[2])
			for i in range(route_data.index(curve_start_node) + 1, route_length):

				route_node_x = route_data[i]["x"]
				route_node_y = route_data[i]["y"]
				if route_node_x == result[1] and route_node_y == result[2]:
					break
				tiles[route_node_x + route_node_y * map_width] = k_empty_tile_id


			curve_start_node_index = curve_start_node_index + curves_interval

		else:

			curve_start_node_index = curve_start_node_index + 1

		curves_iterations_num = curves_iterations_num + 1
		if curves_iterations_num >= route_length or curve_start_node_index >= (route_length - 1):
			break


	lights = []
	is_light_top_side = 0
	is_light_left_side = 0
	route_points = "";
	for path_it in route_data:

		route_points = route_points + str(path_it["x"] * k_tile_size + k_tile_size * 0.5) + "," + str(path_it["y"] * k_tile_size + k_tile_size * 0.5) + " "
		if path_it["id"] == k_straight_road_up_down_id:

			location_x = path_it["x"] * k_tile_size + k_tile_size * 0.5
			if is_light_left_side == 0:

				is_light_left_side = 1
				location_x = location_x - k_tile_size * 0.5

			else:

				is_light_left_side = 0
				location_x = location_x + k_tile_size * 0.5

			lights.append({"x": location_x, "y": path_it["y"] * k_tile_size + k_tile_size * 0.5})

		if path_it["id"] == k_straight_road_left_right_id:

			location_y = path_it["y"] * k_tile_size + k_tile_size * 0.5
			if is_light_top_side == 0:

				is_light_top_side = 1
				location_y = location_y - k_tile_size * 0.5

			else:

				is_light_top_side = 0
				location_y = location_y + k_tile_size * 0.5

			lights.append({"x": path_it["x"] * k_tile_size + k_tile_size * 0.5, "y": location_y})

	
	map_node = ElementTree.Element("map", version="1.2", tiledversion="1.2.2", orientation="orthogonal", renderorder="right-down", width=str(map_width), height=str(map_height), tilewidth=str(k_tile_size), tileheight=str(k_tile_size), infinite="0", nextlayerid="1", nextobjectid="1")
	tileset_node = ElementTree.SubElement(map_node, "tileset", firstgid="1", source="../gbResources/track_01_tileset.tsx")
	track_layer_node = ElementTree.SubElement(map_node, "layer", id="1", name="track", width=str(map_width), height=str(map_height))
	track_data_node = ElementTree.SubElement(track_layer_node, "data")

	for tile in tiles:

		track_tile_node = ElementTree.SubElement(track_data_node, "tile", gid=str(tile))

	walls_objectgroup_node = ElementTree.SubElement(map_node, "objectgroup", id="1", name="walls")

	index = 0
	for wall_it in walls:

		index = index + 1
		wall_node = ElementTree.SubElement(walls_objectgroup_node, "object", id=str(index), x=str(wall_it["x"]), y=str(wall_it["y"]))
		wall_polyline_node = ElementTree.SubElement(wall_node, "polyline", points=str(wall_it["points"]))

	route_objectgroup_node = ElementTree.SubElement(map_node, "objectgroup", id="2", name="route")
	index = index + 1
	route_node = ElementTree.SubElement(route_objectgroup_node, "object", id=str(index), x=str(0), y=str(0))
	route_polyline_node = ElementTree.SubElement(route_node, "polyline", points=str(route_points))

	lights_objectgroup_node = ElementTree.SubElement(map_node, "objectgroup", id="3", name="lights")
	for light_it in lights:
		index = index + 1
		light_node = ElementTree.SubElement(lights_objectgroup_node, "object", id=str(index), x=str(light_it["x"]), y=str(light_it["y"]))

	track = ElementTree.ElementTree(map_node)
	track.write("track_output.tmx")


if __name__ == "__main__":
    main(sys.argv[1:])
