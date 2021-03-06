/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */
#pragma once

#include <ewol/widget/Windows.hpp>
#include <ewol/widget/Layer.hpp>
#include <ewol/widget/Composer.hpp>
#include <audio/algo/drain/BiQuadType.hpp>
#include <appl/widget/DisplayFrequency.hpp>

namespace appl {
	class Windows : public ewol::widget::Windows {
		protected:
			Windows();
			void init();
		public:
			DECLARE_FACTORY(Windows);
		protected:
			ememory::SharedPtr<ewol::widget::Composer> m_gui;
			ememory::SharedPtr<appl::widget::DisplayFrequency> m_displayer;
			void onCallbackSampleRateLow();
			void onCallbackSampleRateUp();
			void onCallbackTypeUp();
			void onCallbackTypeLow();
			void onCallbackGain(const etk::String& _value);
			void onCallbackGainSlider(const float& _value);
			void onCallbackFrequency(const etk::String& _value);
			void onCallbackFrequencySlider(const float& _value);
			void onCallbackQuality(const etk::String& _value);
			void onCallbackQualitySlider(const float& _value);
			void onCallbackStart();
			void onCallbackStart16();
			void onCallbackStartFloat();
		protected:
			int32_t m_sampleRate;
			etk::Vector<int32_t> m_listSampleRate;
			enum audio::algo::drain::biQuadType m_type;
			etk::Vector<enum audio::algo::drain::biQuadType> m_listType;
			float m_cutFrequency;
			float m_gain;
			float m_quality;
	};
}

