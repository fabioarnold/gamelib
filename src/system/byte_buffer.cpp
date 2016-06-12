void ByteBuffer::appendUInt8(uint8_t i) {
	_data.push_back(i);
}

void ByteBuffer::appendSInt32LE(int32_t i) {
	_data.reserve(_data.size() + 4); // alloc
	_data.push_back((uint8_t)( i        & 0xFF));
	_data.push_back((uint8_t)((i >>  8) & 0xFF));
	_data.push_back((uint8_t)((i >> 16) & 0xFF));
	_data.push_back((uint8_t)((i >> 24) & 0xFF));
}

void ByteBuffer::appendData(uint8_t *data, int dataSize) {
	_data.reserve(_data.size() + dataSize); // alloc
	for (int i = 0; i < dataSize; i++) {
		_data.push_back(data[i]);
	}
}

void ByteBuffer::setSInt32LE(int index, int32_t i) {
	*(int32_t*)&(_data.data()[index]) = i;
/* slower
	_data[index    ] = (uint8_t)( i        & 0xFF);
	_data[index + 1] = (uint8_t)((i >>  8) & 0xFF);
	_data[index + 2] = (uint8_t)((i >> 16) & 0xFF);
	_data[index + 3] = (uint8_t)((i >> 24) & 0xFF);
 */
}

int ByteBuffer::getLength() const {
	return (int)_data.size();
}

uint8_t ByteBuffer::getUInt8(int index) const {
	return _data.data()[index];
}

int32_t ByteBuffer::getSInt32LE(int index) const {
	return *(int32_t*)&(_data.data()[index]);
	/*
	return _data[index]
		| (_data[index + 1] << 8)
		| (_data[index + 2] << 16)
		| (_data[index + 3] << 24);
	 */
}

uint8_t *ByteBuffer::getData(int index) const {
	return (uint8_t *)&(_data.data()[index]);
}
