var documents = fl.documents;
if (documents && documents.length)
{
	var global_wall_index = 0;
	var documents_count = documents.length;
	for (var i = 0; i < documents_count; i++)
	{
		var document = documents[i];
		if (document)
		{
			var library_object = document.library;
			if (library_object)
			{
				var library_items = library_object.items;
				if (library_items)
				{
					for(var j = 0; j < library_items.length; j++)
					{
						var library_item = library_items[j];
						if (library_item.itemType == "movie clip")
						{
							if(library_item.linkageClassName == "level")
							{
								library_object.editItem(library_item.name);
								var timeline = fl.getDocumentDOM().getTimeline();
								if (timeline)
								{
									var layers_count = timeline.layers.length;
									for(var k = 0; k < layers_count; k++)
									{
										if(timeline.layers[k].name == "walls")
										{
											var walls_layer = timeline.layers[k];
											var walls_elements = walls_layer.frames[0].elements;
											var walls_elements_count = walls_elements.length;
											for(var z = 0; z < walls_elements_count; z++)
											{
												var walls_element = walls_elements[z];
												walls_element.name = "wall_" + global_wall_index;
												global_wall_index++;
												fl.trace(global_wall_index);
											}
										}
										fl.trace(timeline.layers[k].name);
									}
								}
								fl.trace(library_item);
							}
						}
					}
				}
			}
		}
	}
}