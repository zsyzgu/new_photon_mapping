古裔正 zsyzgu@163.com

一、简介

这是一个光线追踪引擎，可以根据描述场景的数据渲染出3D图像，并导出在bmp文件中

场景中支持的物品形状只有平面和球，光源只有点光源和矩形光源，可以通过派生类添加其它的形状

在计算阴影和镜面漫反射时可以使用蒙特卡罗算法使之更真实

二、运行环境

应该可以在不同环境下编译通过，然后就可以运行了

三、样例程序

``` cpp
#include"raytracer.h"
int main() {
	Raytracer* raytracer = new Raytracer;
	raytracer->SetInput( "scene.txt" );
	raytracer->SetOutput( "picture.bmp" );
	raytracer->Run();
	delete raytracer;
	return 0;
}
```

四、样例输入文件

``` cpp
//双斜杠"//"和井号"#"都可以注释。
//以下打“*”号的是一定要有的信息，否则不必要有或者已有缺省信息。
background			#设置背景
	color= 0.05 0.05 0.05		//背景颜色					[缺省值(0,0,0)]
end				#结束设置背景

camera				#设置摄像机信息
	O= -2 2 0.5			//感光点的位置					[*]
	N= 0.6 1 -0.6			//摄像机的朝向					[*]
	image_H= 760			//bmp图像的象素高				[缺省值420]
	image_W= 1280			//bmp图像的象素宽				[缺省值420]
	lens_H= 0.6			//摄像机镜头的高（与镜头到感光点距离之比）	[缺省值0.88]
	lens_W= 1.0			//摄像机镜头的宽				[缺省值0.88]
	shade_quality= 4		//计算面积光源光照的质量			[缺省值4]
	drefl_quality= 4		//计算镜面漫反射的质量				[缺省值4]
	max_photons= 5000000		//光子图可容纳光子数				[缺省值2000000]
	emit_photons= 20000000		//总发射光子数					[缺省值1000000]
	sample_photons= 500		//采样时所需光子数				[缺省值100]
	sample_dist= 1			//采样时最大半径				[缺省值1]
end

light area			#设置一个矩形光源
	O= 3 3 3			//矩形光源的中心点				[*]
	Dx= 1.5 0 0			//矩形光源的x半轴				[*]
	Dy= 0 1.5 0			//矩形光源的y半轴				[*]
	color= 25 25 25			//光源的色光					[*]
end

primitive sphere		#设置一个球（一个透明的玻璃球）
	O= -0.5 5.14 -1.8		//球形						[*]
	R= 0.2				//半径						[*]
	refr= 1				//折射所占百分比				[缺省值0]
	rindex= 1.7			//折射率					[*有折射时必须]
	absor= 1 0 0			//吸收色光的强度				[*有折射时必须]
end

primitive sphere		#设置一个球（一个透明的玻璃球）
	O= 0 5 -1.8
	R= 0.2
	refr= 1
	rindex= 1.7
	absor= 0 1 0
end

primitive sphere		#设置一个球（一个透明的玻璃球）
	O= 0.5 5.14 -1.8
	R= 0.2
	refr= 1
	rindex= 1.7
	absor= 0 0 1
end

primitive sphere		#设置一个球（一个镜面反射的球）
	O= 1.5 6.3 -1.6
	R= 0.4
	color= 0.5 0.5 1
	refl= 1				//镜面反射所占百分比				[缺省值0]
end

primitive sphere		#设置一个球（一个大理石球）
	O= 0 6 -1.5
	R= 0.5
	color= 1 1 1
	texture= marble.bmp		//有纹理，设置纹理所在bmp文件			[*有纹理时必须]
	De= 0 0 1			//球的z轴					[缺省值(0,0,1)仅在有纹理时可选]
	Dc= 0 1 0			//球的极轴					[缺省值(0,1,0)仅在有纹理时可选]
	diff= 0.7			//漫反射百分比					[缺省值0]			
	refl= 0.3
	drefl= 0.25			//蒙特卡罗算法计算镜面漫反射时反射光的偏差半径	[缺省值0，值为0时不使用该算法]
end

primitive plane			#设置一个球（木地板）
	N= 0 0 1			//平面的法向量（正方向）			[*]
	R= -2				//原点到平面的距离				[*]
	color= 1 1 1			
	diff= 0.5
	refl= 0.5
	drefl= 0.15
	texture= floor.bmp
	Dx= 8 0 0			//平面的x轴，其长度有意义：其纹理在场景中的长	[*有纹理时必须]
	Dy= 0 8 0			//平面的y轴					[*有纹理时必须]
end
```

五、对色光、光子能量的说明

1：物体的各色光分量均不能大于1，(1,1,1)减去色光的值是被物体吸收的部分；

2：光源的各色光分量可以大于1，色光能量为(r,g,b)的光源能使1米以外正对光源的物体得到(r,g,b)的色光；光的能量与传播距离的平方成反比。
