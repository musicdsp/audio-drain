/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */
#pragma once

#include <audio/drain/EndPoint.hpp>
#include <etk/Function.hpp>
#include <ethread/Mutex.hpp>
#include <audio/drain/CircularBuffer.hpp>

namespace audio {
	namespace drain{
		typedef etk::Function<void (const audio::Time& _time,
		                              size_t _nbChunk,
		                              enum audio::format _format,
		                              uint32_t _frequency,
		                              const etk::Vector<audio::channel>& _map)> playbackFunctionWrite;
		class EndPointWrite : public EndPoint {
			private:
				audio::drain::CircularBuffer m_buffer;
				playbackFunctionWrite m_function;
				ethread::Mutex m_mutex;
			protected:
				/**
				 * @brief Constructor
				 */
				EndPointWrite();
				void init();
			public:
				static ememory::SharedPtr<audio::drain::EndPointWrite> create();
				/**
				 * @brief Destructor
				 */
				virtual ~EndPointWrite() {};
				virtual void configurationChange();
				virtual bool process(audio::Time& _time,
				                     void* _input,
				                     size_t _inputNbChunk,
				                     void*& _output,
				                     size_t& _outputNbChunk);
				virtual void write(const void* _value, size_t _nbChunk);
				virtual void setCallback(playbackFunctionWrite _function) {
					m_function = _function;
				}
			protected:
				echrono::microseconds m_bufferSizeMicroseconds; // 0 if m_bufferSizeChunk != 0
				size_t m_bufferSizeChunk; // 0 if m_bufferSizeMicroseconds != 0
				size_t m_bufferUnderFlowSize; //!< Limit display of underflow in the write callback
			public:
				/**
				 * @brief Set buffer size in chunk number
				 * @param[in] _nbChunk Number of chunk in the buffer
				 */
				virtual void setBufferSize(size_t _nbChunk);
				/**
				 * @brief Set buffer size size of the buffer with the stored time in �s
				 * @param[in] _time Time in microsecond of the buffer
				 */
				virtual void setBufferSize(const echrono::microseconds& _time);
				/**
				 * @brief get buffer size in chunk number
				 * @return Number of chunk that can be written in the buffer
				 */
				virtual size_t getBufferSize();
				/**
				 * @brief Set buffer size size of the buffer with the stored time in �s
				 * @return Time in microsecond that can be written in the buffer
				 */
				virtual echrono::microseconds getBufferSizeMicrosecond();
				/**
				 * @brief Get buffer size filled in chunk number
				 * @return Number of chunk in the buffer (that might be read/write)
				 */
				virtual size_t getBufferFillSize();
				/**
				 * @brief Set buffer size size of the buffer with the stored time in �s
				 * @return Time in microsecond of the buffer (that might be read/write)
				 */
				virtual echrono::microseconds getBufferFillSizeMicrosecond();
		};
	}
}

