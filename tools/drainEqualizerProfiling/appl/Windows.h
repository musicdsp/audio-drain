/**
 * @author Edouard DUPIN
 * 
 * @copyright 2010, Edouard DUPIN, all right reserved
 * 
 * @license APACHE-2 (see license file)
 */

#ifndef __APPL_WINDOWS_H__
#define __APPL_WINDOWS_H__

#include <ewol/widget/Windows.h>
#include <ewol/widget/Layer.h>
#include <ewol/widget/Composer.h>
#include <drain/Equalizer.h>
#include <appl/widget/DisplayFrequency.h>

namespace appl {
	class Windows : public ewol::widget::Windows {
		protected:
			Windows();
			void init();
		public:
			DECLARE_FACTORY(Windows);
		protected:
			std::shared_ptr<ewol::widget::Composer> m_gui;
			std::shared_ptr<appl::widget::DisplayFrequency> m_displayer;
			void onCallbackSampleRateLow();
			void onCallbackSampleRateUp();
			void onCallbackTypeUp();
			void onCallbackTypeLow();
			void onCallbackGain(const std::string& _value);
			void onCallbackGainSlider(const float& _value);
			void onCallbackFrequency(const std::string& _value);
			void onCallbackFrequencySlider(const float& _value);
			void onCallbackQuality(const std::string& _value);
			void onCallbackQualitySlider(const float& _value);
			void onCallbackStart();
		protected:
			int32_t m_sampleRate;
			std::vector<int32_t> m_listSampleRate;
			enum drain::filterType m_type;
			std::vector<enum drain::filterType> m_listType;
			float m_cutFrequency;
			float m_gain;
			float m_quality;
			std::vector<std::pair<float,float> > calculateTheory(double _sampleRate, std::vector<float> _coef);
	};
};


#endif