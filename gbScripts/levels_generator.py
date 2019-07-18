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
k_corner_road_up_left_points_0 = "112,-128 96,-64 64,-32 0,-16"
k_corner_road_up_left_points_1 = "16,-128 14,-120 8,-114 0,-112"
k_corner_road_right_up_points_0 = "16,-128 32,-64 64,-32 128,-16"
k_corner_road_right_up_points_1 = "112,-128 114,-120 120,-114 128,-112"

k_office_buildings_2x2_ground_floor = ["office_01_ground_floor", "office_02_ground_floor", "office_03_ground_floor", "office_04_ground_floor"]
k_office_buildings_1x2_ground_floor = ["office_05_ground_floor", "office_06_ground_floor", "office_07_ground_floor", "office_08_ground_floor"]

k_office_buildings_2x2_floor = ["office_01_floor", "office_02_floor", "office_03_floor", "office_04_floor"]
k_office_buildings_1x2_floor = ["office_05_floor", "office_06_floor", "office_07_floor", "office_08_floor"]

k_appartment_01_buildings_1x2_ground_floor = ["appartment_ground_floor_01", "appartment_ground_floor_02", "appartment_ground_floor_03", "appartment_ground_floor_04"]
k_appartment_01_buildings_1x2_roof = ["appartment_roof_01", "appartment_roof_01", "appartment_roof_01", "appartment_roof_01"]


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

						should_generate_corner = random.random() > 0.75 and iteration < 2
						if should_generate_corner:

							result = generate_next_corner(initial_direction_id, next_route_node_x, next_route_node_y, map_data, map_width, map_height, additional_route_data, iteration)
							iteration = result[0]
							direction_x = result[1]
							direction_y = result[2]
							next_direction_id = result[3]
							next_route_node = result[4]

						else:

							map_data[next_route_node_x + next_route_node_y * map_width] = next_direction_id
							next_route_node = {"x": next_route_node_x, "y": next_route_node_y, "id": next_direction_id}
							additional_route_data.append(next_route_node)


						result = generate_curve(initial_direction_id, initial_x, initial_y, next_direction_id, direction_x, direction_y, map_data, route_data, next_route_node, additional_route_data, map_width, map_height, iteration)
						is_generated = result[0]
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
						is_generated = result[0]
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
					is_generated = result[0]
					end_curve_route_node_x = result[1]
					end_curve_route_node_y = result[2]
			
			else:

				map_data[next_route_node_x + next_route_node_y * map_width] = next_direction_id
				next_route_node = {"x": next_route_node_x, "y": next_route_node_y, "id": next_direction_id}
				additional_route_data.append(next_route_node)
				result = generate_curve(initial_direction_id, initial_x, initial_y, next_direction_id, direction_x, direction_y, map_data, route_data, next_route_node, additional_route_data, map_width, map_height, iteration)
				is_generated = result[0]
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


def can_capture_placement(map_data, map_width, map_height, x , y, captured_placements):

	if map_data[x + y * map_width] == k_empty_tile_id and captured_placements[x + y * map_width] == 0:
		return 1

	return 0

def generate_building_placement_2x2(map_data, map_width, map_height, x , y, captured_placements):

	if can_capture_placement(map_data, map_width, map_height, x , y, captured_placements):
		placement = {"rotation": 0, "points":[{"x": x, "y": y}]};
	else:

		placement = {"rotation": 0, "points":[]};
		return placement

	potential_placement = {"x": -1, "y": -1};

	for i in range(x - 1, x + 1):

		for j in range(y - 1, y + 1):

			if i == x and j == y:
				continue

			if i > 1 and i < (map_width - 2) and j > 1 and j < (map_height - 2):

				if map_data[i + j * map_width] == k_empty_tile_id and captured_placements[i + j * map_width] == 0:

					potential_placement = {"x": i, "y": j}
					if placement["points"][0]["x"] == potential_placement["x"]:

						if (i + 1) > 1 and (i + 1) < (map_width - 2) and map_data[(i + 1) + j * map_width] == k_empty_tile_id and map_data[(i + 1) + y * map_width] == k_empty_tile_id and captured_placements[(i + 1) + j * map_width] == 0 and captured_placements[(i + 1) + y * map_width] == 0:

							placement["points"].append(potential_placement)
							placement["points"].append({"x": i + 1, "y": j})
							placement["points"].append({"x": i + 1, "y": y})

							return placement

						elif (i - 1) > 1 and (i - 1) < (map_width - 2) and map_data[(i - 1) + j * map_width] == k_empty_tile_id and map_data[(i - 1) + y * map_width] == k_empty_tile_id and captured_placements[(i - 1) + j * map_width] == 0 and captured_placements[(i - 1) + y * map_width] == 0:

							placement["points"].append(potential_placement)
							placement["points"].append({"x": i - 1, "y": j})
							placement["points"].append({"x": i - 1, "y": y})

							return placement

					elif placement["points"][0]["y"] == potential_placement["y"]:

						if (j + 1) > 1 and (j + 1) < (map_height - 2) and map_data[i + (j + 1) * map_width] == k_empty_tile_id and map_data[i + (y + 1) * map_width] == k_empty_tile_id and captured_placements[i + (j + 1) * map_width] == 0 and captured_placements[i + (y + 1) * map_width] == 0:

							placement["points"].append(potential_placement)
							placement["points"].append({"x": i, "y": j + 1})
							placement["points"].append({"x": i, "y": y + 1})

							return placement

						elif (j - 1) > 1 and (j - 1) < (map_height - 2) and map_data[i + (j - 1) * map_width] == k_empty_tile_id and map_data[i + (y - 1) * map_width] == k_empty_tile_id and captured_placements[i + (j - 1) * map_width] == 0 and captured_placements[i + (y - 1) * map_width] == 0:

							placement["points"].append(potential_placement)
							placement["points"].append({"x": i, "y": j - 1})
							placement["points"].append({"x": i, "y": y - 1})

							return placement

	return placement


def generate_building_placement_1x2(map_data, map_width, map_height, x , y, captured_placements, one_side):

	if can_capture_placement(map_data, map_width, map_height, x , y, captured_placements):
		placement = {"rotation": 0, "points":[{"x": x, "y": y}]};
	else:

		placement = {"rotation": 0, "points":[]};
		return placement

	potential_placement = {"x": -1, "y": -1};

	for i in range(x - 1, x + 1):

		for j in range(y - 1, y + 1):

			if i == x and j == y:
				continue

			if i > 1 and i < (map_width - 2) and j > 1 and j < (map_height - 2) and (i == x or j == y):

				if map_data[i + j * map_width] == k_empty_tile_id and captured_placements[i + j * map_width] == 0:

					potential_placement = {"x": i, "y": j}
					can_place = 0;

					if placement["points"][0]["x"] == i:

						if one_side == 1:

							if placement["points"][0]["y"] > j:

								if placement["points"][0]["y"] + 1 < map_height - 1 and map_data[i + (placement["points"][0]["y"] + 1) * map_width] != k_empty_tile_id:

									placement["rotation"] = 180
									can_place = 1

								if j - 1 >= 0 and  map_data[i + (j - 1) * map_width] != k_empty_tile_id:

									placement["rotation"] = 0
									can_place = 1

						else:

							placement["rotation"] = 0
							can_place = 1
							
					elif placement["points"][0]["y"] == j:

						if one_side == 1:

							if placement["points"][0]["x"] > i:

								if placement["points"][0]["x"] + 1 < map_width - 1 and  map_data[(placement["points"][0]["x"] + 1) + j * map_width] != k_empty_tile_id:

									placement["rotation"] = 90
									can_place = 1

								if i - 1 >= 0 and  map_data[(i - 1) + j * map_width] != k_empty_tile_id:

									placement["rotation"] = 270
									can_place = 1

						else:

							placement["rotation"] = 90
							can_place = 1


					if can_place == 1:

						placement["points"].append(potential_placement)
						return placement

	return placement


def generate_buildings_2x2(map_data, map_width, map_height, buildings, captured_placements, min_x, min_y, max_x, max_y):

	for x in range(min_x, max_x, 1):

		for y in range(min_y, max_y, 1):

			if map_data[x + y * map_width] == k_empty_tile_id:

				placement = generate_building_placement_2x2(map_data, map_width, map_height, x, y, captured_placements)
				if len(placement["points"]) == 4:
					max_bound = {"x": -1, "y": -1}
					min_bound = {"x": map_width, "y": map_height}
					for placement_it in placement["points"]:


						if placement_it["x"] > max_bound["x"]:
							max_bound["x"] = placement_it["x"]

						if placement_it["y"] > max_bound["y"]:
							max_bound["y"] = placement_it["y"]

						if placement_it["x"] < min_bound["x"]:
							min_bound["x"] = placement_it["x"]

						if placement_it["y"] < min_bound["y"]:
							min_bound["y"] = placement_it["y"]

						captured_placements[placement_it["x"] + placement_it["y"] * map_width] = 1

					min_bound["x"] = min_bound["x"] * k_tile_size - k_tile_size * 0.5
					min_bound["y"] = min_bound["y"] * k_tile_size - k_tile_size * 0.5
					max_bound["x"] = max_bound["x"] * k_tile_size - k_tile_size * 0.5
					max_bound["y"] = max_bound["y"] * k_tile_size - k_tile_size * 0.5

					center = {"x": (min_bound["x"] + k_tile_size * 1.5), "y": (min_bound["y"] + k_tile_size * 1.5)}
					random_index = random.randrange(0, len(k_office_buildings_2x2_ground_floor) - 1, 1)

					buildings.append({"center": center, "rotation": placement["rotation"], "size": 4, "name": k_office_buildings_2x2_ground_floor[random_index]});
					buildings.append({"center": center, "rotation": placement["rotation"], "size": 4, "name": k_office_buildings_2x2_floor[random_index]});

def generate_buildings_1x2(map_data, map_width, map_height, buildings, captured_placements, min_x, min_y, max_x, max_y, buildings_ground_floors, buildings_floors, one_side):

	for x in range(min_x, max_x, 1):

		for y in range(min_y, max_y, 1):

			if map_data[x + y * map_width] == k_empty_tile_id:

				placement = generate_building_placement_1x2(map_data, map_width, map_height, x, y, captured_placements, one_side)
				if len(placement["points"]) == 2:

					max_bound = {"x": -1, "y": -1}
					min_bound = {"x": map_width, "y": map_height}
					for placement_it in placement["points"]:

						if placement_it["x"] > max_bound["x"]:
							max_bound["x"] = placement_it["x"]

						if placement_it["y"] > max_bound["y"]:
							max_bound["y"] = placement_it["y"]

						if placement_it["x"] < min_bound["x"]:
							min_bound["x"] = placement_it["x"]

						if placement_it["y"] < min_bound["y"]:
							min_bound["y"] = placement_it["y"]

						captured_placements[placement_it["x"] + placement_it["y"] * map_width] = 1

					min_bound["x"] = min_bound["x"] * k_tile_size - k_tile_size * 0.5
					min_bound["y"] = min_bound["y"] * k_tile_size - k_tile_size * 0.5
					max_bound["x"] = max_bound["x"] * k_tile_size - k_tile_size * 0.5
					max_bound["y"] = max_bound["y"] * k_tile_size - k_tile_size * 0.5

					if placement["points"][0]["x"] == placement["points"][1]["x"]:
						
						center = {"x": (min_bound["x"] + k_tile_size), "y": (min_bound["y"] + k_tile_size * 1.5)}

					elif placement["points"][0]["y"] == placement["points"][1]["y"]:

						center = {"x": (min_bound["x"] + k_tile_size * 1.5), "y": (min_bound["y"] + k_tile_size)}

					random_index = random.randrange(0, len(buildings_ground_floors) - 1, 1)

					buildings.append({"center": center, "rotation": placement["rotation"], "size": 2, "name": buildings_ground_floors[random_index]});
					if len(buildings_ground_floors) == len(buildings_floors):

						buildings.append({"center": center, "rotation": placement["rotation"], "size": 2, "name": buildings_floors[random_index]});

def generate_random_zone_buildings(map_data, map_width, map_height, buildings, captured_placements, zones_placements, zone_name, zone_index):

	if zone_name == "big_business":
		generate_buildings_2x2(map_data, map_width, map_height, buildings, captured_placements, zones_placements[zone_index]["a"], zones_placements[zone_index]["b"], zones_placements[zone_index]["c"], zones_placements[zone_index]["d"])
	elif zone_name == "small_bussines":
		generate_buildings_1x2(map_data, map_width, map_height, buildings, captured_placements, zones_placements[zone_index]["a"], zones_placements[zone_index]["b"], zones_placements[zone_index]["c"], zones_placements[zone_index]["d"], k_office_buildings_1x2_ground_floor, k_office_buildings_1x2_floor, 0)
	elif zone_name == "appartments":
		generate_buildings_1x2(map_data, map_width, map_height, buildings, captured_placements, zones_placements[zone_index]["a"], zones_placements[zone_index]["b"], zones_placements[zone_index]["c"], zones_placements[zone_index]["d"], k_appartment_01_buildings_1x2_ground_floor, k_appartment_01_buildings_1x2_roof, 1)
	

def generate_buildings(map_data, map_width, map_height, buildings):

	zones_names = ["big_business", "small_bussines", "appartments"]

	captured_placements = []

	for x in range(0, map_width, 1):

		for y in range(0, map_height, 1):

			captured_placements.append(0)



	zones_placements = [{"a": 2, "b": 2, "c": int(map_width * 0.5), "d": int(map_height * 0.5)}, {"a": int(map_width * 0.5), "b": 2, "c": map_width, "d": int(map_height * 0.5)}, {"a": 2, "b": int(map_height * 0.5), "c": int(map_width * 0.5), "d": map_height}, {"a": int(map_width * 0.5), "b": int(map_height * 0.5), "c": map_width, "d": map_height}];
	random_shuffle = random.SystemRandom()
	random_shuffle.shuffle(zones_placements)

	random_zone_index = random.randrange(0, len(zones_names), 1)

	generate_random_zone_buildings(map_data, map_width, map_height, buildings, captured_placements, zones_placements, zones_names[random_zone_index], 0)
	random_zone_index = random.randrange(0, len(zones_names), 1)

	generate_random_zone_buildings(map_data, map_width, map_height, buildings, captured_placements, zones_placements, zones_names[random_zone_index], 1)
	random_zone_index = random.randrange(0, len(zones_names), 1)

	generate_random_zone_buildings(map_data, map_width, map_height, buildings, captured_placements, zones_placements, zones_names[random_zone_index], 2)
	random_zone_index = random.randrange(0, len(zones_names), 1)

	generate_random_zone_buildings(map_data, map_width, map_height, buildings, captured_placements, zones_placements, zones_names[random_zone_index], 3)

	generate_buildings_1x2(map_data, map_width, map_height, buildings, captured_placements, 2, 2, map_width, map_height, k_office_buildings_1x2_ground_floor, k_office_buildings_1x2_floor, 0)


def generate_spawners(route_data, spawners):

	straight_route_length = 0
	for route_data_it in route_data:

		if route_data_it["id"] == k_straight_road_left_right_id or route_data_it["id"] == k_straight_road_up_down_id:

			straight_route_length = straight_route_length + 1

			x_offset = 0;
			y_offset = 0;

			if route_data_it["id"] == k_straight_road_left_right_id:

				y_offset = k_tile_size * 0.15


			if route_data_it["id"] == k_straight_road_up_down_id:

				x_offset = k_tile_size * 0.15

			if straight_route_length == 4:

				index = route_data.index(route_data_it)
				spawners.append({"x": route_data[index]["x"] * k_tile_size + k_tile_size * 0.5 + x_offset, "y": route_data[index]["y"] * k_tile_size + k_tile_size * 0.5 + y_offset})
				spawners.append({"x": route_data[index - 1]["x"] * k_tile_size + k_tile_size * 0.5 - x_offset, "y": route_data[index - 1]["y"] * k_tile_size + k_tile_size * 0.5 - y_offset})
				spawners.append({"x": route_data[index - 2]["x"] * k_tile_size + k_tile_size * 0.5 + x_offset, "y": route_data[index - 2]["y"] * k_tile_size + k_tile_size * 0.5 + y_offset})
				spawners.append({"x": route_data[index - 3]["x"] * k_tile_size + k_tile_size * 0.5 - x_offset, "y": route_data[index - 3]["y"] * k_tile_size + k_tile_size * 0.5 - y_offset})
				break

		else:

			straight_route_length = 0;

def mix(value_1, value_2, alpha):
    return value_1 * (1.0 - alpha) + value_2 * alpha


def main(argv):

	max_levels_count = 0
	complexity_iterations = 0
	level_index = 0

	for argument in argv[:]:

		if "max_levels_count_" in argument:

			max_levels_count = argument[len("max_levels_count_"):]
			print "max levels count:"
			print max_levels_count

		elif "complexity_iterations_" in argument:

			complexity_iterations = argument[len("complexity_iterations_"):]
			print "complexity iterations:"
			print complexity_iterations

		elif "level_index_" in argument:

			level_index = argument[len("level_index_"):]
			print "level index:"
			print level_index

	map_width = random.randrange(k_min_map_size, k_max_map_size, 1)
	map_height = random.randrange(k_min_map_size, k_max_map_size, 1)
	tiles_count = map_width * map_height

	levels_per_complexity_iteration = float(max_levels_count) / float(complexity_iterations)
	current_complexity = (float(level_index) / levels_per_complexity_iteration) - int((float(level_index) - 1.0) / levels_per_complexity_iteration)
	print "current complexity:"
	print current_complexity

	level_round_time = int(mix(random.randrange(3, 6, 1), random.randrange(9, 10, 1), current_complexity)) * 10
	print "level round time:"
	print level_round_time
	
	curves_num = int(mix(2, 18, current_complexity))
	print "curves count: "
	print curves_num

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

			if i > k_offset_from_border and i < ((map_width - 1) - k_offset_from_border) and j == ((map_height - 1) - k_offset_from_border):

				tile_id = k_straight_road_left_right_id

			if j > k_offset_from_border and j < ((map_height - 1) - k_offset_from_border) and i == k_offset_from_border:

				tile_id = k_straight_road_up_down_id

			if j > k_offset_from_border and j < ((map_height - 1) - k_offset_from_border) and i == ((map_width - 1) - k_offset_from_border):

				tile_id = k_straight_road_up_down_id

			if i == k_offset_from_border and j == k_offset_from_border:

				tile_id = k_corner_road_down_right

			if i == k_offset_from_border and j == ((map_height - 1) - k_offset_from_border):

				tile_id = k_corner_road_right_up

			if i == ((map_width - 1) - k_offset_from_border) and j == k_offset_from_border:
				
				tile_id = k_corner_road_left_down

			if i == ((map_width - 1) - k_offset_from_border) and j == ((map_height - 1) - k_offset_from_border):
				
				tile_id = k_corner_road_up_left

			tiles[i + j * map_width] = tile_id
			if tile_id != k_empty_tile_id:
				road.append({"x": i, "y": j, "id": tile_id, "marked_iteration": 0})

	leftmost_location = road[0]
	for road_it in road:

		if road_it["x"] <= leftmost_location["x"] and road_it["y"] <= leftmost_location["y"]:
			leftmost_location = road_it


	next_after_leftmost_location = leftmost_location
	for road_it in road:

		if road_it["x"] == leftmost_location["x"] + 1:
			
			next_after_leftmost_location = road_it
			break


	route_data = []
	route_data.append(leftmost_location)
	leftmost_location["marked_iteration"] = 1

	route_data.append(next_after_leftmost_location)
	next_after_leftmost_location["marked_iteration"] = 1

	find_route(route_data, next_after_leftmost_location, road, map_width, map_height)

	route_length = len(route_data)
	curves_interval = int(route_length / curves_num)
	curve_start_node_index = curves_interval
	curves_iterations_num = 0
	additional_routes_data = []

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

					for additional_route_data_it in additional_route_data:

						additional_route_node_x = additional_route_data_it["x"]
						additional_route_node_y = additional_route_data_it["y"]
						tiles[additional_route_node_x + additional_route_node_y * map_width] = k_empty_tile_id;

					tiles[curve_start_node_x + curve_start_node_y * map_width] = old_direction_id


		if result[0] != 0:

			removed_route_data = []
			last_additional_route_node = additional_route_data.pop()
			additional_route_data.reverse()

			for i in range(route_data.index(curve_start_node) + 1, route_length):

				route_node_x = route_data[i]["x"]
				route_node_y = route_data[i]["y"]
				if route_node_x == result[1] and route_node_y == result[2]:

					route_data[i]["id"] = last_additional_route_node["id"]
					curve_start_node_index = i;
					break
				removed_route_data.append(route_data[i])
			
			additional_routes_data.append({"additional_route": additional_route_data, "removed_route": removed_route_data, "index": route_data.index(curve_start_node) + 1})
			curve_start_node_index = curve_start_node_index + curves_interval

		else:

			curve_start_node_index = curve_start_node_index + 1

		curves_iterations_num = curves_iterations_num + 1
		if curves_iterations_num >= route_length or curve_start_node_index >= (route_length - 1):
			break

	offset = 0
	for additional_routes_data_it in additional_routes_data:

		additional_route_data = additional_routes_data_it["additional_route"]
		index = offset + additional_routes_data_it["index"]
		for additional_route_data_it in additional_route_data:

			route_data.insert(index, additional_route_data_it)

		offset = offset + len(additional_route_data)

	for additional_routes_data_it in additional_routes_data:

		removed_route_data = additional_routes_data_it["removed_route"]
		for removed_route_data_it in removed_route_data:

			for route_data_it in route_data:

				if removed_route_data_it["x"] == route_data_it["x"] and removed_route_data_it["y"] == route_data_it["y"]:

					tiles[route_data_it["x"] + route_data_it["y"] * map_width] = k_empty_tile_id
					route_data.remove(route_data_it)


	for i in range(int(map_width)):

		for j in range(int(map_height)):

			direction_id = tiles[i + j * map_width]
			if direction_id == k_straight_road_left_right_id:
				
				add_straight_road_up_down_walls(walls, i, j)

			elif direction_id == k_straight_road_up_down_id:
				
				add_straight_road_left_right_walls(walls, i, j)

			elif direction_id == k_corner_road_down_right:

				add_corner_road_down_right_walls(walls, i, j)

			elif direction_id == k_corner_road_right_up:

				add_corner_road_right_up_walls(walls, i, j)

			elif direction_id == k_corner_road_left_down:
					
				add_corner_road_left_down_walls(walls, i, j)

			elif direction_id == k_corner_road_up_left:
					
				add_corner_road_up_left_walls(walls, i, j)


	buildings = []
	generate_buildings(tiles, map_width, map_height, buildings)

	spawners = []
	generate_spawners(route_data, spawners)


	lights = []
	slow_motion_triggers = [];
	is_light_top_side = 0
	is_light_left_side = 0
	route_points = "";
	for route_data_it in route_data:

		route_position_x = route_data_it["x"] * k_tile_size + k_tile_size * 0.5
		route_position_y = route_data_it["y"] * k_tile_size + k_tile_size * 0.5

		index = route_data_it["x"] + route_data_it["y"] * map_width
		direction_id = tiles[index]
		route_data_it["id"] = direction_id

		if direction_id == k_corner_road_down_right:

			slow_motion_triggers.append({"x": route_position_x + k_tile_size * 0.25, "y": route_position_y + k_tile_size * 0.25})
			route_position_x = route_position_x - k_tile_size * 0.15
			route_position_y = route_position_y - k_tile_size * 0.15
			lights.append({"x": route_position_x, "y": route_position_y})


		elif direction_id == k_corner_road_left_down:

			slow_motion_triggers.append({"x": route_position_x - k_tile_size * 0.25, "y": route_position_y + k_tile_size * 0.25})
			route_position_x = route_position_x + k_tile_size * 0.15
			route_position_y = route_position_y - k_tile_size * 0.15
			lights.append({"x": route_position_x, "y": route_position_y})

		elif direction_id == k_corner_road_up_left:

			slow_motion_triggers.append({"x": route_position_x - k_tile_size * 0.25, "y": route_position_y - k_tile_size * 0.25})
			route_position_x = route_position_x + k_tile_size * 0.15
			route_position_y = route_position_y + k_tile_size * 0.15
			lights.append({"x": route_position_x, "y": route_position_y})

		elif direction_id == k_corner_road_right_up:

			slow_motion_triggers.append({"x": route_position_x + k_tile_size * 0.25, "y": route_position_y - k_tile_size * 0.25})
			route_position_x = route_position_x - k_tile_size * 0.15
			route_position_y = route_position_y + k_tile_size * 0.15
			lights.append({"x": route_position_x, "y": route_position_y})


		route_points = route_points + str(route_position_x) + "," + str(route_position_y) + " "
		if direction_id == k_straight_road_up_down_id:

			location_x = route_data_it["x"] * k_tile_size + k_tile_size * 0.5
			if is_light_left_side == 0:

				is_light_left_side = 1
				location_x = location_x - k_tile_size * 0.4

			else:

				is_light_left_side = 0
				location_x = location_x + k_tile_size * 0.4

			lights.append({"x": location_x, "y": route_data_it["y"] * k_tile_size + k_tile_size * 0.5})

		if direction_id == k_straight_road_left_right_id:

			location_y = route_data_it["y"] * k_tile_size + k_tile_size * 0.5
			if is_light_top_side == 0:

				is_light_top_side = 1
				location_y = location_y - k_tile_size * 0.4

			else:

				is_light_top_side = 0
				location_y = location_y + k_tile_size * 0.4

			lights.append({"x": route_data_it["x"] * k_tile_size + k_tile_size * 0.5, "y": location_y})

	
	map_node = ElementTree.Element("map", version="1.2", tiledversion="1.2.2", orientation="orthogonal", renderorder="right-down", width=str(map_width), height=str(map_height), tilewidth=str(k_tile_size), tileheight=str(k_tile_size), infinite="0", nextlayerid="1", nextobjectid="1")
	ElementTree.SubElement(map_node, "tileset", firstgid="1", source="../../gbResources/set_tiles.tsx")
	ElementTree.SubElement(map_node, "tileset", firstgid="134", source="../../gbResources/set_objects.tsx")
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

	slow_motion_triggers_objectgroup_node = ElementTree.SubElement(map_node, "objectgroup", id="4", name="slow_motion_triggers")
	for slow_motion_trigger_it in slow_motion_triggers:
		index = index + 1
		slow_motion_triggers_node = ElementTree.SubElement(slow_motion_triggers_objectgroup_node, "object", id=str(index), x=str(slow_motion_trigger_it["x"]), y=str(slow_motion_trigger_it["y"]))

	buildings_objectgroup_node = ElementTree.SubElement(map_node, "objectgroup", id="5", name="buildings")
	for building_it in buildings:
		index = index + 1
		building_node = ElementTree.SubElement(buildings_objectgroup_node, "object", id=str(index), x=str(building_it["center"]["x"]), y=str(building_it["center"]["y"]), rotation=str(building_it["rotation"]), name=building_it["name"])

	spawners_objectgroup_node = ElementTree.SubElement(map_node, "objectgroup", id="6", name="spawners")
	for spawner_it in spawners:
		index = index + 1
		spawner_node = ElementTree.SubElement(spawners_objectgroup_node, "object", id=str(index), x=str(spawner_it["x"]), y=str(spawner_it["y"]))

	level_tmx = ElementTree.ElementTree(map_node)
	level_tmx.write("../gbBundle/scenes/level_" + level_index + ".tmx")

	level_configuration_node = ElementTree.Element("level", scene_filename="level_" + level_index + ".tmx", complexity=str(current_complexity), cars_count="2", session_time_in_seconds=str(level_round_time))
	level_configuration_xml = ElementTree.ElementTree(level_configuration_node)
	level_configuration_xml.write("../gbBundle/configurations/gameplay/level_" + level_index + ".xml")


if __name__ == "__main__":
    main(sys.argv[1:])
