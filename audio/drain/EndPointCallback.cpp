/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */

#include <audio/drain/EndPointCallback.hpp>
#include <audio/drain/debug.hpp>


audio::drain::EndPointCallback::EndPointCallback() :
  m_outputFunction(null),
  m_inputFunction(null) {
	
}

void audio::drain::EndPointCallback::init(playbackFunction _callback) {
	m_outputFunction = _callback;
	audio::drain::EndPoint::init();
	m_type = "EndPointCallback";
}
void audio::drain::EndPointCallback::init(recordFunction _callback) {
	m_inputFunction = _callback;
	audio::drain::EndPoint::init();
	m_type = "EndPointCallback";
}

ememory::SharedPtr<audio::drain::EndPointCallback> audio::drain::EndPointCallback::create(playbackFunction _callback) {
	ememory::SharedPtr<audio::drain::EndPointCallback> tmp(ETK_NEW(audio::drain::EndPointCallback));
	tmp->init(_callback);
	return tmp;
}

ememory::SharedPtr<audio::drain::EndPointCallback> audio::drain::EndPointCallback::create(recordFunction _callback) {
	ememory::SharedPtr<audio::drain::EndPointCallback> tmp(ETK_NEW(audio::drain::EndPointCallback));
	tmp->init(_callback);
	return tmp;
}

void audio::drain::EndPointCallback::configurationChange() {
	audio::drain::EndPoint::configurationChange();
	DRAIN_INFO("update : format=" << m_output.getFormat() << " map=" << m_input.getMap() << " freq=" << m_input.getFrequency() << " size=" << int32_t(m_formatSize));
	m_needProcess = true;
}


bool audio::drain::EndPointCallback::process(audio::Time& _time,
                                             void* _input,
                                             size_t _inputNbChunk,
                                             void*& _output,
                                             size_t& _outputNbChunk){
	audio::drain::AutoLogInOut tmpLog("EndPointCallback");
	if (m_inputFunction != null) {
		// Call user ...
		DRAIN_VERBOSE("call user set " << _inputNbChunk << "*" << m_input.getMap().size());
		m_inputFunction(_input,
		                _time,
		                _inputNbChunk,
		                m_output.getFormat(),
		                m_output.getFrequency(),
		                m_output.getMap());
		return true;
	}
	if (m_outputFunction != null) {
		// update buffer size ...
		m_outputData.resize(_inputNbChunk*m_output.getMap().size()*m_formatSize);
		// clean output to prevent errors ...
		memset(&m_outputData[0], 0, m_outputData.size());
		// call user
		DRAIN_VERBOSE("call user get " << _inputNbChunk << "*" << m_output.getMap().size() << " map=" << m_output.getMap() << " datasize=" << int32_t(m_formatSize));
		m_outputFunction(&m_outputData[0],
		                 _time,
		                 _inputNbChunk,
		                 m_output.getFormat(),
		                 m_output.getFrequency(),
		                 m_output.getMap());
		if (m_outputData.size() != _inputNbChunk*m_output.getMap().size()*m_formatSize) {
			DRAIN_ERROR(" can not get enough data from user ... " << m_outputData.size() << " != " << _inputNbChunk*m_output.getMap().size());
			return false;
		}
		_output = &m_outputData[0];
		_outputNbChunk = _inputNbChunk;
		return true;
	}
	return false;
}

