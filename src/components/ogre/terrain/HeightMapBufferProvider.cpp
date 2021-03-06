/*
 Copyright (C) 2009 Erik Ogenvik <erik@ogenvik.org>

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

#include "HeightMapBufferProvider.h"
#include "HeightMapBuffer.h"
#include "Buffer.h"

namespace Ember {
namespace OgreView {

namespace Terrain {

HeightMapBufferProvider::HeightMapBufferProvider(unsigned int bufferResolution, unsigned int desiredBuffers, unsigned int desiredBuffersTolerance) :
		mBufferResolution(bufferResolution), mDesiredBuffers(desiredBuffers), mDesiredBuffersTolerance(desiredBuffersTolerance) {
	while (mPrimitiveBuffers.size() < mDesiredBuffers) {
		mPrimitiveBuffers.emplace_back(new Buffer<float>(mBufferResolution, 1));
	}
}

HeightMapBufferProvider::~HeightMapBufferProvider() = default;

void HeightMapBufferProvider::checkin(std::unique_ptr<Buffer<float>> buffer) {
	mPrimitiveBuffers.push_back(std::move(buffer));
}

HeightMapBuffer* HeightMapBufferProvider::checkout() {
	if (mPrimitiveBuffers.empty()) {
		while (mPrimitiveBuffers.size() < mDesiredBuffers) {
			mPrimitiveBuffers.emplace_back(new Buffer<float>(mBufferResolution, 1));
		}
	}
	auto buffer = std::move(mPrimitiveBuffers.back());
	mPrimitiveBuffers.pop_back();
	return new HeightMapBuffer(*this, std::move(buffer));
}

void HeightMapBufferProvider::maintainPool() {

	if (mPrimitiveBuffers.size() <= mDesiredBuffers - mDesiredBuffersTolerance) {
		while (mPrimitiveBuffers.size() < mDesiredBuffers) {
			mPrimitiveBuffers.emplace_back(new Buffer<float>(mBufferResolution, 1));
		}
	}

	if (mPrimitiveBuffers.size() >= mDesiredBuffers + mDesiredBuffersTolerance) {
		while (mPrimitiveBuffers.size() > mDesiredBuffers) {
			mPrimitiveBuffers.pop_back();
		}
	}
}

}

}
}
