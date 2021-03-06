/*
 Copyright (C) 2009 Erik Ogenvik

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software Foundation,
 Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "TerrainShaderParser.h"

#include "components/ogre/terrain/TerrainHandler.h"
#include "components/ogre/terrain/TerrainShader.h"
#include "components/ogre/terrain/TerrainLayerDefinition.h"
#include "components/ogre/terrain/TerrainLayerDefinitionManager.h"

#include <Mercator/Shader.h>
#include <Mercator/FillShader.h>
#include <Mercator/ThresholdShader.h>
#include <Mercator/DepthShader.h>
#include <Mercator/GrassShader.h>
#include <Mercator/ShaderFactory.h>

#include <Atlas/Message/Element.h>

namespace Ember {
namespace OgreView {

namespace Terrain {

TerrainShaderParser::TerrainShaderParser(TerrainHandler& terrainHandler) :
		mTerrainHandler(terrainHandler) {
}


TerrainShaderParser::~TerrainShaderParser() = default;

//namespace {
//float extractFloat(const Atlas::Message::ListType& params, size_t position) {
//	if (params.size() > position) {
//		const Atlas::Message::Element& elem(params[position]);
//		if (elem.isNum()) {
//			return elem.asNum();
//		}
//	}
//	return 0;
//}
//}

void TerrainShaderParser::createShaders(const Atlas::Message::Element& terrain) {
	//For now don't allow updating of shaders.
	//TODO: add support for altering terrain shaders
	if (!mTerrainHandler.getAllShaders().empty()) {
		return;
	}

	Terrain::TerrainLayerDefinitionManager& terrainManager = Terrain::TerrainLayerDefinitionManager::getSingleton();
	bool isValid = false;
	if (terrain.isMap()) {
		auto& terrainMap = terrain.Map();
		auto Isurfaces = terrainMap.find("surfaces");
		if (Isurfaces != terrainMap.end()) {
			auto& surfaces = Isurfaces->second;
			if (surfaces.isList()) {
				auto& slist = surfaces.List();
				for (const auto& surfaceElement : slist) {
					if (surfaceElement.isMap()) {
						auto& surfaceMap = surfaceElement.Map();

						Mercator::Shader::Parameters params;
						if (surfaceMap.count("params")) {
							const Atlas::Message::Element& paramsElem(surfaceMap.find("params")->second);
							if (paramsElem.isMap()) {
								for (const auto& param : paramsElem.asMap()) {
									if (param.second.isNum()) {
										params[param.first] = param.second.asNum();
									}
								}
							}
						}

						auto Iname = surfaceMap.find("name");
						if (Iname != surfaceMap.end()) {
							auto& nameElem = Iname->second;
							if (nameElem.isString()) {
								auto& name = nameElem.String();
								auto def = terrainManager.getDefinitionForShader(name);
								if (def) {
									auto Ipattern = surfaceMap.find("pattern");
									if (Ipattern != surfaceMap.end()) {
										auto& patternElem = Ipattern->second;
										if (patternElem.isString()) {
											const std::string& pattern = patternElem.String();
											Mercator::Shader* shader = Mercator::ShaderFactories::instance().newShader(pattern, params);
											if (shader) {
												isValid = true;
												mTerrainHandler.createShader(def, shader);
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}


	if (!isValid) {
		createDefaultShaders();
	}
}

void TerrainShaderParser::createDefaultShaders() {
	Terrain::TerrainLayerDefinitionManager& terrainManager = Terrain::TerrainLayerDefinitionManager::getSingleton();
	Terrain::TerrainLayerDefinition* def(0);
	if ((def = terrainManager.getDefinitionForShader("rock"))) {
		mTerrainHandler.createShader(def, new Mercator::FillShader());
	}
	if ((def = terrainManager.getDefinitionForShader("sand"))) {
		mTerrainHandler.createShader(def, new Mercator::BandShader(-2.f, 1.5f));
	}

	if ((def = terrainManager.getDefinitionForShader("grass"))) {
		mTerrainHandler.createShader(def, new Mercator::GrassShader(1.f, 80.f, .5f, 1.f));
	}
}

}

}
}
