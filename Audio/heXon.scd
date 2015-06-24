s.boot();
//Raw Shot
(
{
	var ampEnv = Env(
		levels: [0,0.5,0.1,0.2,0,0.5,0],
		times:[0.027,0.01,0.005,0.01,0.005,0.1],
		curve: 'sin');
	var freqEnv = Env(
		levels: [[10,10],[650,623],[10,20],[230,200]],
		times:[0.07],
		curve:1);
	Pulse.ar(EnvGen.kr(freqEnv),Pulse.kr(100, SinOsc.kr(300,0,0.8, 0.2),0.2,0.5),EnvGen.kr(ampEnv, doneAction:1));
}.play();
)

//Blink
(
{
	var ampEnv = Env(
		levels: [0,0.5,0],
		times:[0.08,0.08],
		curve: 1);
	var freqEnv = Env(
		levels: [[190,220],[220,190]],
		times:[0.1],
		curve:1);
	Pulse.ar(EnvGen.kr(freqEnv),0.5,EnvGen.kr(ampEnv, doneAction:1));
}.play();
)

//Shoot
(
{
	var ampEnv = Env(
		levels: [0,0.5,0],
		times:[0.015,0.07],
		curve: 1);
	var freqEnv = Env(
		levels: [900,400],
		times:[0.1],
		curve:1);
	Saw.ar(EnvGen.kr(freqEnv),EnvGen.kr(ampEnv, doneAction: 1),[0,0]);
}.play();
)

//Hit
(
{
	var ampEnv = Env(
		levels: [0,0.5,0],
		times:[0.01,0.06],
		curve: 1);
	var freqEnv = Env(
		levels: [100,110],
		times:[0.07],
		curve:1);
	Pulse.ar(EnvGen.kr(freqEnv),Pulse.kr([100,101], SinOsc.kr([100,102],0,0.25, 0.5),0.25,0.5),EnvGen.kr(ampEnv, doneAction:1));
}.play();
)

//Fire
(
{
var ampenv = Env(
		levels:[0,2,0],
		times:[0.08,0.6],
		curve:-1
	);

var freqenv = Env(
		[2,0,4,0],
		[0.15,0.5],
		curve:-8
	);

	SinOsc.ar(
		Pulse.kr(100,0.1,50,100)*EnvGen.kr(freqenv),
		Saw.kr(80*EnvGen.kr(freqenv),WhiteNoise.kr(8),0),
		SinOsc.kr(Pulse.kr(20,0.5,2,80),[0,3.14],0.01,0.01)*WhiteNoise.kr(3,1)
	)*EnvGen.kr(ampenv, doneAction:2);
}.play();
)