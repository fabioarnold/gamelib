// right now just a convenience wrapper for std::vector
#include <vector>
// TODO: replace usage of std::vector

struct ByteBuffer {
	void clear() {_data.clear();}

	void appendUInt8(uint8_t i);
	void appendSInt32LE(int32_t i);
	void appendData(uint8_t *data, int dataSize);

	void setSInt32LE(int index, int32_t i);

	int getLength() const;

	uint8_t getUInt8(int index) const;
	int getSInt32LE(int index) const;
	uint8_t *getData(int index) const;

	std::vector<uint8_t> _data;
};
