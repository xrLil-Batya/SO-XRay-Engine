#ifndef SGM_SHADERS_CONFIG_H_INCLUDED
#define SGM_SHADERS_CONFIG_H_INCLUDED

//////////////////////////////////////////////////////////////////////////////////////////
//developer section
	#define I_AM_DEVELOPER		//включает режим разработчика, даёт возможность настраивать эффекты с постфиксом debug через консоль

//////////////////////////////////////////////////////////////////////////////////////////
//sunshafts
	#define SUNSHAFTS_SAMPLES int(24)		//количество выборок, качество лучей. Чем больше значение, тем меньше "ребристости". Улучшает качество эффекта, но влияет на производительность
//	#define SUNSHAFTS_LENGTH float(0.05) 		//плотность семплов, длинна лучей. Чем больше значение, тем длиннее лучи и больше "ребристости"
//	#define SUNSHAFTS_RADIUS float(1) 		//радиус распространения лучей на экранном пространстве
//	#define SUNSHAFTS_INTENSITY float(1) 		//интенсивность эффекта. Чем больше значение, тем ярче лучи
//	#define SUNSHAFTS_BLEND_FACTOR float(0.05) 		//фактор смешивания лучей с рассеянным светом
	#define USE_SUNSHAFTS_BLUR		//включить блюр для маски лучей перед выборкой. Улучшает качество эффекта, но влияет на производительность
	#define SUNSHAFTS_BLUR_SAMPLES int(14)		//количество выборок блюра. Улучшает качество эффекта, но влияет на производительность

#endif//SGM_SHADERS_CONFIG_H_INCLUDED