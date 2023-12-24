#include "plugin.hpp"
#include <componentlibrary.hpp>

struct PelogQuantizer : Module {
enum ParamId {
		ENUMS(NOTES, 7),
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
		ENUMS(NOTE_LIGHTS,7),
		LIGHTS_LEN
	};

	PelogQuantizer() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		for(int i=0;i<7;i++){
			configSwitch(NOTES+i,0.f,1.f,0.f, string::f("Note %d",i+1));

		}
		configParam(ROOT_PARAM, -1.f, 1.f, 0.f, "Root pitch");
		configParam(OCTAVE_PARAM, -5.f, 5.f, 0.f, "Octave shift");
		paramQuantities[OCTAVE_PARAM]->snapEnabled=true;
		configInput(ROOT_INPUT, "Root");
		configInput(OCTAVE_INPUT, "Octave");
		configInput(VOLTAGE_INPUT, "Voltage");
		configOutput(VOLTAGE_OUTPUT, "Quantized");
	}

	float pelog_frequency[7]={0,120.0/1206.0,258.0/1206.0,539.0/1206.0,675.0/1206.0,785.0/1206.0,943.0/1206.0};

	void filterNotes(bool selected_notes[7]){
		bool none_selected=true;
		for(int i=0;i<7;i++){
			bool note = params[NOTES+i].getValue()>0.f;
			lights[NOTE_LIGHTS+i].setBrightness(note*0.3);
			if(note){
				selected_notes[i]=true; //TODO: tambah if none selected
				none_selected=false;
			}
			else{
				selected_notes[i]=false;
			}
		}
		if(none_selected){
			for(int i=0;i<7;i++){
				selected_notes[i]=true;
			}
		}

		
	}

	float applyOctaveOffset(float raw_output){ //TODO: tambah if connected statement
		if(inputs[OCTAVE_INPUT].isConnected()){
			return raw_output+static_cast<int>(inputs[OCTAVE_INPUT].getVoltage())*1.005f;
		} 
		else{
			return raw_output+params[OCTAVE_PARAM].getValue()*1.005f;
		}
	}

	float applyPitchOffset(float raw_output){ //TODO: tambah if connected statement
		if(inputs[ROOT_INPUT].isConnected()){
			return raw_output+inputs[ROOT_INPUT].getVoltage()*1.005f;
		}
		else{
			return raw_output+params[ROOT_PARAM].getValue()*1.005f;
		}
	}

	float quantize(float input_volts, bool selected_notes[7]){
		// Ambil nada voltage 0-1
		int octave = int(floorf(input_volts));
		float pitch = input_volts - octave;

		// Cari nada pelog yang paling dekat dengan input
		float closest=5.0;
		float closest_diff=5.0;
		float diff=5.0;
		int played_button=-1;

		for(int i=0; i<7; i++){
			if(selected_notes[i]){
				diff=fabs(pitch-pelog_frequency[i]);
				if(diff<closest_diff){
					closest=pelog_frequency[i];
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
			bool selected_notes[7];
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



struct PelogQuantizerWidget : ModuleWidget {
	PelogQuantizerWidget(PelogQuantizer* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/PelogQuantizer.svg")));

		addParam(createLightParamCentered<smallLightLatch<SmallSimpleLight<WhiteLight>>>(mm2px(Vec(10.16, 14.135)), module, PelogQuantizer::NOTES+6, PelogQuantizer::NOTE_LIGHTS+6));
		addParam(createLightParamCentered<smallLightLatch<SmallSimpleLight<WhiteLight>>>(mm2px(Vec(10.16, 19.275)), module, PelogQuantizer::NOTES+5, PelogQuantizer::NOTE_LIGHTS+5));
		addParam(createLightParamCentered<smallLightLatch<SmallSimpleLight<WhiteLight>>>(mm2px(Vec(10.16, 24.415)), module, PelogQuantizer::NOTES+4, PelogQuantizer::NOTE_LIGHTS+4));
		addParam(createLightParamCentered<smallLightLatch<SmallSimpleLight<WhiteLight>>>(mm2px(Vec(10.16, 29.555)), module, PelogQuantizer::NOTES+3, PelogQuantizer::NOTE_LIGHTS+3));
		addParam(createLightParamCentered<smallLightLatch<SmallSimpleLight<WhiteLight>>>(mm2px(Vec(10.16, 34.695)), module, PelogQuantizer::NOTES+2, PelogQuantizer::NOTE_LIGHTS+2));
		addParam(createLightParamCentered<smallLightLatch<SmallSimpleLight<WhiteLight>>>(mm2px(Vec(10.16, 39.835)), module, PelogQuantizer::NOTES+1, PelogQuantizer::NOTE_LIGHTS+1));
		addParam(createLightParamCentered<smallLightLatch<SmallSimpleLight<WhiteLight>>>(mm2px(Vec(10.16, 44.975)), module, PelogQuantizer::NOTES+0, PelogQuantizer::NOTE_LIGHTS+0));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(10.16, 61.664)), module, PelogQuantizer::ROOT_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(10.16, 83.525)), module, PelogQuantizer::OCTAVE_PARAM));

		addInput(createInputCentered<TinyPJ301MPort>(mm2px(Vec(10.16, 69.089)), module, PelogQuantizer::ROOT_INPUT));
		addInput(createInputCentered<TinyPJ301MPort>(mm2px(Vec(10.16, 90.95)), module, PelogQuantizer::OCTAVE_INPUT));
		addInput(createInput<TinyPJ301MPort>(mm2px(Vec(2.54, 102.8)), module, PelogQuantizer::VOLTAGE_INPUT));

		addOutput(createOutput<TinyPJ301MPort>(mm2px(Vec(12.606, 102.8)), module, PelogQuantizer::VOLTAGE_OUTPUT));
	}
};


Model* modelPelogQuantizer = createModel<PelogQuantizer, PelogQuantizerWidget>("PelogQuantizer");