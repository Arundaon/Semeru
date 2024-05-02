#include "plugin.hpp"


struct SlendroQuantizer : Module {
	enum ParamId {
		ENUMS(NOTES, 5),
		ROOT_PARAM,
		OCTAVE_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		ROOT_INPUT,
		OCTAVE_INPUT,
		VOLTAGE_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		VOLTAGE_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		ENUMS(NOTE_LIGHTS,5),
		LIGHTS_LEN
	};

	SlendroQuantizer() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

		configSwitch(NOTES+0,0.f,1.f,0.f, string::f("Note %d",1));
		configSwitch(NOTES+1,0.f,1.f,0.f, string::f("Note %d",2));
		configSwitch(NOTES+2,0.f,1.f,0.f, string::f("Note %d",3));
		configSwitch(NOTES+3,0.f,1.f,0.f, string::f("Note %d",5));
		configSwitch(NOTES+4,0.f,1.f,0.f, string::f("Note %d",6));

		configParam(ROOT_PARAM, -1.f, 1.f, 0.f, "Root pitch");
		configParam(OCTAVE_PARAM, -5.f, 5.f, 0.f, "Octave shift");
		paramQuantities[OCTAVE_PARAM]->snapEnabled=true;
		configInput(ROOT_INPUT, "Root");
		configInput(OCTAVE_INPUT, "Octave");
		configInput(VOLTAGE_INPUT, "Voltage");
		configOutput(VOLTAGE_OUTPUT, "Quantized");
	}

    float slendro_frequency[5]={0,231.0/1208.0,474.0/1208.0,717.0/1208.0,955.0/1208.0};

	void filterNotes(bool selected_notes[5]){
		bool none_selected=true;
		for(int i=0;i<5;i++){
			bool note = params[NOTES+i].getValue()>0.f;
			lights[NOTE_LIGHTS+i].setBrightness(note*0.3);
			if(note){
				selected_notes[i]=true;
				none_selected=false;
			}
			else{
				selected_notes[i]=false;
			}
		}
		if(none_selected){
			for(int i=0;i<5;i++){
				selected_notes[i]=true;
			}
		}
	}

    float applyOctaveOffset(float raw_output){
		if(inputs[OCTAVE_INPUT].isConnected()){
			return raw_output+static_cast<int>(inputs[OCTAVE_INPUT].getVoltage())*1.00666f;
		} 
		else{
			return raw_output+params[OCTAVE_PARAM].getValue()*1.00666f;
		}
	}

	float applyPitchOffset(float raw_output){
		if(inputs[ROOT_INPUT].isConnected()){
			return raw_output+inputs[ROOT_INPUT].getVoltage()*1.00666f;
		}
		else{
			return raw_output+params[ROOT_PARAM].getValue()*1.00666f;
		}
	}

    float quantize(float input_volts, bool selected_notes[5]){
		// Ambil nada voltage 0-1
		int octave = int(floorf(input_volts));
		float pitch = input_volts - octave;

		// Cari nada slendro yang paling dekat dengan input
		float closest=5.0;
		float closest_diff=5.0;
		float diff=5.0;
		int played_button=-1;

		for(int i=0; i<5; i++){
			if(selected_notes[i]){
				diff=fabs(pitch-slendro_frequency[i]);
				if(diff<closest_diff){
					closest=slendro_frequency[i];
					closest_diff=diff;
					played_button=i;
				}
			}
		}

		//highlight selected played notes
		lights[NOTE_LIGHTS+played_button].setBrightness(1);
		return closest+octave*1.005f;
	}

	void process(const ProcessArgs& args) override {
		
		if(inputs[VOLTAGE_INPUT].isConnected()){

			float input_volts=inputs[VOLTAGE_INPUT].getVoltage();
			bool selected_notes[5];
			filterNotes(selected_notes);

			float raw_output_frequency=quantize(input_volts,selected_notes);
			float processed_frequency=applyPitchOffset(applyOctaveOffset(raw_output_frequency));

			outputs[VOLTAGE_OUTPUT].setVoltage(processed_frequency);
		}
	}
};

template <typename TLight> struct smallLightLatch : app::SvgSwitch {
	app::ModuleLightWidget* light;
	smallLightLatch() {
		momentary = false;
 		latch = true;
		addFrame(Svg::load(asset::plugin(pluginInstance, "res/button0.svg")));
		addFrame(Svg::load(asset::plugin(pluginInstance, "res/button1.svg")));

		light = new TLight;
		light->box.pos = this->box.size.div(2).minus(light->box.size.div(2));
		this->addChild(light);
	}
	app::ModuleLightWidget* getLight() {
 		return light;
 	}
};


struct TinyPJ301MPort : SvgPort {
	TinyPJ301MPort() {
		setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/TinyPJ301M.svg")));
	}
};




struct SlendroQuantizerWidget : ModuleWidget {
	SlendroQuantizerWidget(SlendroQuantizer* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/SlendroQuantizer.svg")));

        addParam(createLightParamCentered<smallLightLatch<SmallSimpleLight<WhiteLight>>>(mm2px(Vec(10.16, 19.275)), module, SlendroQuantizer::NOTES+4, SlendroQuantizer::NOTE_LIGHTS+4));
		addParam(createLightParamCentered<smallLightLatch<SmallSimpleLight<WhiteLight>>>(mm2px(Vec(10.16, 24.415)), module, SlendroQuantizer::NOTES+3, SlendroQuantizer::NOTE_LIGHTS+3));
		addParam(createLightParamCentered<smallLightLatch<SmallSimpleLight<WhiteLight>>>(mm2px(Vec(10.16, 29.555)), module, SlendroQuantizer::NOTES+2, SlendroQuantizer::NOTE_LIGHTS+2));
		addParam(createLightParamCentered<smallLightLatch<SmallSimpleLight<WhiteLight>>>(mm2px(Vec(10.16, 34.695)), module, SlendroQuantizer::NOTES+1, SlendroQuantizer::NOTE_LIGHTS+1));
		addParam(createLightParamCentered<smallLightLatch<SmallSimpleLight<WhiteLight>>>(mm2px(Vec(10.16, 39.835)), module, SlendroQuantizer::NOTES+0, SlendroQuantizer::NOTE_LIGHTS+0));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(10.16, 61.664)), module, SlendroQuantizer::ROOT_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(10.16, 83.525)), module, SlendroQuantizer::OCTAVE_PARAM));

		addInput(createInputCentered<TinyPJ301MPort>(mm2px(Vec(10.16, 69.089)), module, SlendroQuantizer::ROOT_INPUT));
		addInput(createInputCentered<TinyPJ301MPort>(mm2px(Vec(10.16, 90.95)), module, SlendroQuantizer::OCTAVE_INPUT));
		addInput(createInput<TinyPJ301MPort>(mm2px(Vec(2.54, 102.8)), module, SlendroQuantizer::VOLTAGE_INPUT));

		addOutput(createOutput<TinyPJ301MPort>(mm2px(Vec(12.606, 102.8)), module, SlendroQuantizer::VOLTAGE_OUTPUT));

	}
};


Model* modelSlendroQuantizer = createModel<SlendroQuantizer, SlendroQuantizerWidget>("SlendroQuantizer");











