/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */

#include <audio/drain/CircularBuffer.hpp>
#include <audio/drain/debug.hpp>

audio::drain::CircularBuffer::CircularBuffer(const audio::drain::CircularBuffer& _obj) :
  m_data(),
  m_write(null),
  m_read(null),
  m_timeRead(),
  m_capacity(0),
  m_sizeChunk(0),
  m_size(0) {
	DRAIN_CRITICAL("error");
};
/**
 * @brief copy operator.
 */
audio::drain::CircularBuffer& audio::drain::CircularBuffer::operator=(const audio::drain::CircularBuffer& _obj) {
	DRAIN_CRITICAL("error");
	return *this;
};

audio::drain::CircularBuffer::CircularBuffer() :
  m_data(),
  m_write(null),
  m_read(null),
  m_timeRead(),
  m_capacity(0),
  m_sizeChunk(0),
  m_size(0) {
	// nothing to do ...
}

audio::drain::CircularBuffer::~CircularBuffer() {
	m_data.clear();
	m_read = null;
	m_write = null;
}

void audio::drain::CircularBuffer::setCapacity(size_t _capacity, size_t _chunkSize, uint32_t _frequency) {
	if (    _chunkSize == m_sizeChunk
	     && _capacity == m_capacity) {
		clear();
		return;
	}
	DRAIN_DEBUG("buffer setCapacity(" << _capacity << "," << _chunkSize << ")");
	if (_capacity == 0) {
		DRAIN_ERROR("set a buffer capacity with 0 data ... (reset default at 4096)");
		_capacity = 4096;
	}
	if (_chunkSize == 0) {
		DRAIN_ERROR("set a buffer capacity with chunksize = 0 ... (reset default at 8)");
		_chunkSize = 8;
	}
	m_data.clear();
	m_write = null;
	m_read = null;
	m_frequency = _frequency;
	m_capacity = _capacity;
	m_sizeChunk = _chunkSize;
	m_size = 0;
	if (    _capacity == 0
	     || _chunkSize == 0) {
		m_capacity = 0;
		m_sizeChunk = 0;
		return;
	}
	m_data.resize(m_capacity*m_sizeChunk, 0);
	m_read = &m_data[0];
	m_write = &m_data[0];
}

void audio::drain::CircularBuffer::setCapacity(echrono::Duration _capacity, size_t _chunkSize, uint32_t _frequency) {
	uint32_t nbSampleNeeded = _frequency*_capacity.get()/1000000000LL;
	DRAIN_DEBUG("buffer setCapacity(" << _capacity << " ," << _chunkSize << ")");
	setCapacity(nbSampleNeeded, _chunkSize, _frequency);
}


size_t audio::drain::CircularBuffer::getUsedSizeBeforEnd() const {
	size_t size;
	if (m_read < m_write) {
		size = static_cast<uint8_t*>(m_write) - static_cast<uint8_t*>(m_read);
		// the size result is in bytes we need to have it in element
		size /= m_sizeChunk;
	} else if (    m_read == m_write
	            && m_size == 0) {
		// no element in the buffer
		size = 0;
	} else {
		size = &m_data[0] + (m_capacity*m_sizeChunk) - static_cast<uint8_t*>(m_read);
		// the size result is in bytes we need to have it in element
		size /= m_sizeChunk;
	}
	return size;
}

size_t audio::drain::CircularBuffer::getFreeSizeBeforEnd() const {
		size_t size;
		size =   &m_data[0]
		       + (m_capacity*m_sizeChunk)
		       - static_cast<uint8_t*>(m_write);
		// the size result is in Octet we need to have it in element
		size /= m_sizeChunk;
		return size;
}
size_t audio::drain::CircularBuffer::write(const void* _data, size_t _nbChunk) {
	return write(_data, _nbChunk, audio::Time::now());//m_timeRead + )
}

size_t audio::drain::CircularBuffer::write(const void* _data, size_t _nbChunk, const audio::Time& _time) {
	if (m_data.size() == 0) {
		DRAIN_ERROR("EMPTY Buffer");
		return _nbChunk;
	}
	size_t nbElementDrop = 0;
	size_t freeSizeBeforeEnd = getFreeSizeBeforEnd();
	size_t freeSize = m_capacity - m_size;
	// Write element in all case
	// calculate the number of element that are overwritten
	if (freeSize < _nbChunk) {
		nbElementDrop = _nbChunk - freeSize;
	}
	// if User Request a write more important than the size of the buffer ==> update the pointer to feet only on the buffer size
	if (m_capacity < _nbChunk) {
		DRAIN_WARNING("CircularBuffer Write too BIG " << _nbChunk << " buffer max size : " << m_capacity << " (keep last Elements)");
		// Move data pointer
		_data = static_cast<const uint8_t*>(_data) + (_nbChunk - m_capacity) * m_sizeChunk;
		// update size
		_nbChunk = m_capacity;
	}
	// if no element in the FIFO ==> first time write or no more data inside ==> start set the file of the read data ...
	if (m_size == 0) {
		m_timeRead = _time;
	}
	// TODO : Check time to push continuous data ...
	
	if (freeSizeBeforeEnd >= _nbChunk) {
		// all Data will be copy
		memcpy(m_write, _data, _nbChunk * m_sizeChunk);
		// update Writing pointer
		if (freeSizeBeforeEnd == _nbChunk) {
			// update to the end of FIFO ==> update to the start
			m_write = &m_data[0];
		} else {
			m_write = static_cast<uint8_t*>(m_write) + _nbChunk * m_sizeChunk;
		}
		// Update the number of element in the buffer.
		m_size += _nbChunk;
	} else {
		// copy data to the end of buffer
		memcpy(m_write, _data, freeSizeBeforeEnd * m_sizeChunk);
		// update Writing pointer ==> end of buffer ==> go to the start
		m_write = &m_data[0];
		// update data pointer
		_data = static_cast<const uint8_t*>(_data) + freeSizeBeforeEnd * m_sizeChunk;
		m_size += freeSizeBeforeEnd;
		// get the number of element we need to write
		_nbChunk -= freeSizeBeforeEnd;
		// Copy the las data if needed
		if (_nbChunk != 0) {
			memcpy(m_write, _data, _nbChunk * m_sizeChunk);
			// update Writing pointer
			m_write = static_cast<uint8_t*>(m_write) + _nbChunk * m_sizeChunk;
			m_size += _nbChunk;
		}
	}
	if (nbElementDrop > 0) {
		// if drop element we need to update the reading pointer
		m_read = m_write;
		m_size = m_capacity;
	}
	// return the number of element Overwrite
	return nbElementDrop;
}

size_t audio::drain::CircularBuffer::read(void* _data, size_t _nbChunk) {
	return read(_data, _nbChunk, m_timeRead);
}

size_t audio::drain::CircularBuffer::read(void* _data, size_t _nbChunk, const audio::Time& _time) {
	size_t nbElementDrop = 0;
	// Critical section (theoriquely protected by Mutex)
	size_t usedSizeBeforeEnd = getUsedSizeBeforEnd();
	// verify if we have elements in the Buffer
	if (0 < m_size) {
		// check the time of the read :
		audio::Duration deltaTime = m_timeRead - _time;
		if (deltaTime.get() == 0) {
			// nothing to do ==> just copy data ...
		} else if (deltaTime.get() > 0) {
			// Add empty sample in the output buffer ...
			size_t nbSampleEmpty = m_frequency*deltaTime.get()/1000000000LL;
			nbSampleEmpty = etk::min(nbSampleEmpty, _nbChunk);
			DRAIN_WARNING("add Empty sample in the output buffer " << nbSampleEmpty << " / " << _nbChunk);
			memset(_data, 0, nbSampleEmpty * m_sizeChunk);
			if (nbSampleEmpty == _nbChunk) {
				return 0;
			}
			_nbChunk -= nbSampleEmpty;
		} else {
			// Remove data from the FIFO
			setReadPosition(_time);
		}
		if (m_size < _nbChunk) {
			nbElementDrop = _nbChunk - m_size;
			DRAIN_VERBOSE("crop nb sample : m_size=" << m_size << " _nbChunk=" << _nbChunk);
			_nbChunk = m_size;
		}
		m_timeRead += echrono::microseconds(_nbChunk*1000000/m_frequency);
		if (usedSizeBeforeEnd >= _nbChunk) {
			// all Data will be copy
			memcpy(_data, m_read, _nbChunk * m_sizeChunk);
			// update Writing pointer
			m_read = static_cast<uint8_t*>(m_read) + _nbChunk * m_sizeChunk;
			m_size -= _nbChunk;
			// update output pointer in case of flush with 0 data
			_data = static_cast<uint8_t*>(_data) + _nbChunk * m_sizeChunk;
		} else {
			// copy data to the end of buffer
			memcpy(_data, m_read, usedSizeBeforeEnd * m_sizeChunk);
			// update Writing pointer ==> end of buffer ==> go to the start
			m_read = &m_data[0];
			_data = static_cast<uint8_t*>(_data) + usedSizeBeforeEnd * m_sizeChunk;
			m_size -= usedSizeBeforeEnd;
			// get the number of element we need to write
			_nbChunk -= usedSizeBeforeEnd;
			// Copy the last data if needed
			if (0 != _nbChunk) {
				memcpy(_data, m_read, _nbChunk * m_sizeChunk);
				// update Writing pointer
				m_read = static_cast<uint8_t*>(m_read) + _nbChunk * m_sizeChunk;
				m_size -= _nbChunk;
				// update output pointer in case of flush with 0 data
				_data = static_cast<uint8_t*>(_data) + _nbChunk * m_sizeChunk;
			}
		}
	} else {
		nbElementDrop = _nbChunk;
	}
	if (0 < nbElementDrop) {
		// set 0 in last element of the output
		memset(_data, 0, m_sizeChunk * nbElementDrop);
	}
	// return the number of element droped
	return nbElementDrop;
}

void audio::drain::CircularBuffer::setReadPosition(const audio::Time& _time) {
	// Critical section (theoriquely protected by Mutex)
	size_t usedSizeBeforeEnd = getUsedSizeBeforEnd();
	if (0 < m_size) {
		// check the time of the read :
		audio::Duration deltaTime = _time - m_timeRead;
		size_t nbSampleToRemove = int64_t(m_frequency)*int64_t(deltaTime.get())/1000000000LL;
		nbSampleToRemove = etk::min(nbSampleToRemove, m_size);
		DRAIN_VERBOSE("Remove sample in the buffer " << nbSampleToRemove << " / " << m_size);
		audio::Duration updateTime(0,(int64_t(nbSampleToRemove)*1000000000LL)/int64_t(m_frequency));
		DRAIN_VERBOSE(" add time : " << updateTime << " / " << deltaTime);
		if (usedSizeBeforeEnd >= nbSampleToRemove) {
			usedSizeBeforeEnd -= nbSampleToRemove;
			m_size -= nbSampleToRemove;
			m_read = static_cast<uint8_t*>(m_read) + nbSampleToRemove * m_sizeChunk;
		} else {
			nbSampleToRemove -= usedSizeBeforeEnd;
			m_size -= nbSampleToRemove;
			m_read = &m_data[0] + nbSampleToRemove*m_sizeChunk;
		}
		m_timeRead += updateTime;
		//m_timeRead += deltaTime;
	} else {
		m_timeRead = audio::Time();
	}
}


size_t audio::drain::CircularBuffer::getFreeSize() const {
	return m_capacity - m_size;
}

void audio::drain::CircularBuffer::clear() {
	DRAIN_DEBUG("buffer clear()");
	// set pointer to the start
	m_read  = &m_data[0];
	m_write = &m_data[0];
	// Clean the number of element in the buffer
	m_size = 0;
	// Clean all element inside :
	memset(&m_data[0], 0, m_sizeChunk * m_capacity);
}
