/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */
#pragma once

#include <ewol/widget/Widget.hpp>
#include <ewol/compositing/Drawing.hpp>
#include <ewol/compositing/Text.hpp>
#include <ewol/widget/Manager.hpp>

namespace appl {
	namespace widget {
		class DisplayFrequency : public ewol::Widget {
			private:
				ewol::compositing::Drawing m_draw; //!< drawing instance
				ewol::compositing::Text m_text; //!< drawing instance
			protected:
				//! @brief constructor
				DisplayFrequency();
				void init();
			public:
				DECLARE_WIDGET_FACTORY(DisplayFrequency, "DisplayFrequency");
				//! @brief destructor
				virtual ~DisplayFrequency();
			private:
				etk::Vector<etk::Vector<etk::Pair<float,float> > > m_data; //!< data that might be displayed
			public:
				void clear() {
					m_data.clear();
				}
				void setValue(const etk::Vector<etk::Pair<float,float> >& _data);
			private:
				float m_gainMin; //!< display minimum gain value
				float m_gainMax; //!< display maximum gain value
			public:
				void setGainRange(float _min, float _max);
			private:
				float m_frequencyMin; //!< display minimum gain value
				float m_frequencyMax; //!< display maximum gain value
			public:
				void setFrequencyRange(float _min, float _max);
			public: // herited function
				virtual void onDraw();
				virtual void onRegenerateDisplay();
			private:
				vec2 m_borderSize;
		};
	}
}

