# Semeru
<a href="https://www.youtube.com/watch?v=217rQ-XxRmg">Demo Video</a> <br><br>
VCV Rack is an open-source software that simulates hardware modular synthesizers, used by musicians and hobbyists worldwide. The Semeru plugin was created for my thesis to facilitate musicians to use Gamelan’s Pelog and Slendro microtonal scales in their VCV Rack patches easily. It was implemented with C++, with the help of VCV’s SDK.
<p align="center">
  <img src="https://github.com/Arundaon/Semeru/assets/68210759/1d10c79c-d4d4-4f71-9ec2-60645f4d28fd" height="400">
</p>
The plugin consists of two modules, Pelog Quantizer and Slendro Quantizer. The functionalities of the modules are alike, which is to convert the voltage input in VCV Rack into Pelog or Slendro.



## Pelog Quantizer
<p align="center">
<img src="https://github.com/Arundaon/Semeru/assets/68210759/ec61206b-02b5-42e8-bf59-4f886f5a55a0" height="400">
</p>

Pelog Quantizer will quantize the voltage input into the 7-note Pelog scale (1, 2, 3, 4, 5, 6, 7). The module consists of:
1. 7 buttons to select notes to play
2. 1 Root knob and 1 Root input to fine-tune the pitch
3. 1 Octave knob and 1 Octave input to shift the note up or down the octave
4. 1 Voltage input and 1 Voltage output


## Slendro Quantizer
<p align="center">
<img src="https://github.com/Arundaon/Semeru/assets/68210759/45037aec-4249-459c-9cbf-54b5af885cad" height="400">
</p>

Slendro Quantizer will quantize the voltage input into the 5-note Slendro scale (1, 2, 3, 5, 6). The module consists of:
1. 5 buttons to select notes to play
2. 1 Root knob and 1 Root input to fine-tune the pitch
3. 1 Octave knob and 1 Octave input to shift the note up or down the octave
4. 1 Voltage input and 1 Voltage output

## Credit
Thanks to Vectezzy for providing free vector images for the Wayang logo!
<a href="https://www.vecteezy.com/free-vector/wayang">Wayang Vectors by Vecteezy</a>
