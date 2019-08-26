var EN_CH_list = [
    {"English":"3 point color balance","Chinese":"3点平衡"}
    ,{"English":"3dflippo","Chinese":"3D变换"}
    ,{"English":"B","Chinese":"蓝通道"}
    ,{"English":"Baltan","Chinese":"延迟alpha"}
    ,{"English":"BgSubtract0r","Chinese":"去静背景"}
    ,{"English":"bluescreen0r","Chinese":"绿幕抠像"}
    ,{"English":"Brightness","Chinese":"亮度"}
    ,{"English":"bw0r","Chinese":"黑白图像"}
    ,{"English":"c0rners","Chinese":"透视变换"}
    ,{"English":"cairogradient","Chinese":"图像渐变"}
    ,{"English":"cairoimagegrid","Chinese":"图像网格"}
    ,{"English":"Cartoon","Chinese":"卡通特效"}
    ,{"English":"Color Distance","Chinese":"颜色距离"}
    ,{"English":"coloradj_RGB","Chinese":"RGB调节"}
    ,{"English":"colorhalftone","Chinese":"半色调"}
    ,{"English":"colorize","Chinese":"上色"}
    ,{"English":"colortap","Chinese":"预调色彩"}
    ,{"English":"Contrast0r","Chinese":"对比度"}
    ,{"English":"Defish0r","Chinese":"鱼眼"}
    ,{"English":"delay0r","Chinese":"视频延迟"}
    ,{"English":"Delaygrab","Chinese":"延迟帧"}
    ,{"English":"Distort0r","Chinese":"扭曲"}
    ,{"English":"dither","Chinese":"抖动算法"}
    ,{"English":"Edgeglow","Chinese":"边缘发光"}
    ,{"English":"emboss","Chinese":"浮雕"}
    ,{"English":"Equaliz0r","Chinese":"均衡化"}
    ,{"English":"Flippo","Chinese":"翻转"}
    ,{"English":"G","Chinese":"绿通道"}
    ,{"English":"Gamma","Chinese":"伽玛"}
    ,{"English":"Glitch0r","Chinese":"毛刺"}
    ,{"English":"Glow","Chinese":"辉光"}
    ,{"English":"hqdn3d","Chinese":"3D降噪"}
    ,{"English":"Hueshift0r","Chinese":"色调偏移"}
    ,{"English":"IIR blur","Chinese":"IIR模糊"}
    ,{"English":"Invert0r","Chinese":"反色"}
    ,{"English":"K-Means Clustering","Chinese":"k-Means"}
    ,{"English":"keyspillm0pup","Chinese":"keyspill"}
    ,{"English":"Lens Correction","Chinese":"镜头校正"}
    ,{"English":"LetterB0xed","Chinese":"上下黑边"}
    ,{"English":"Levels","Chinese":"通道强度"}
    ,{"English":"Light Graffiti","Chinese":"光涂鸦"}
    ,{"English":"Luminance","Chinese":"亮度"}
    ,{"English":"Mask0Mate","Chinese":"alpha蒙版"}
    ,{"English":"Medians","Chinese":"中值滤波"}
    ,{"English":"NDVI filter","Chinese":"NDVI滤波"}
    ,{"English":"Nervous","Chinese":"及时冲洗"}
    ,{"English":"Nikon D90 Stairstepping fix","Chinese":"D90修复"}
    ,{"English":"Normaliz0r","Chinese":"归一化"}
    ,{"English":"nosync0r","Chinese":"坏信号"}
    ,{"English":"pixeliz0r","Chinese":"像素化"}
    ,{"English":"posterize","Chinese":"色调分离"}
    ,{"English":"pr0be","Chinese":"测量"}
    ,{"English":"pr0file","Chinese":"轮廓"}
    ,{"English":"Premultiply or Unpremultiply","Chinese":"预乘/除"}
    ,{"English":"primaries","Chinese":"主色"}
    ,{"English":"R","Chinese":"红通道"}
    ,{"English":"rgbnoise","Chinese":"Rgb噪音"}
    ,{"English":"rgbsplit0r","Chinese":"Rgb拆分"}
    ,{"English":"Saturat0r","Chinese":"饱和度"}
    ,{"English":"scanline0r","Chinese":"扫描线"}
    ,{"English":"select0r","Chinese":"颜色遮罩"}
    ,{"English":"Sharpness","Chinese":"锐度"}
    ,{"English":"sigmoidaltransfer","Chinese":"去饱和"}
    ,{"English":"Sobel","Chinese":"索贝尔滤镜"}
    ,{"English":"softglow","Chinese":"柔光"}
    ,{"English":"SOP/Sat","Chinese":"颜色校正"}
    ,{"English":"spillsupress","Chinese":"溢出"}
    ,{"English":"Squareblur","Chinese":"方形模糊"}
    ,{"English":"TehRoxx0r","Chinese":"小块闪现"}
    ,{"English":"threelay0r","Chinese":"3级阈值"}
    ,{"English":"Threshold0r","Chinese":"阈值"}
    ,{"English":"Timeout indicator","Chinese":"超时指标"}
    ,{"English":"Tint0r","Chinese":"色调"}
    ,{"English":"Transparency","Chinese":"透明度"}
    ,{"English":"Twolay0r","Chinese":"动态阈值"}
    ,{"English":"Vertigo","Chinese":"混合光晕"}
    ,{"English":"Vignette","Chinese":"渐变光晕"}
    ,{"English":"White Balance","Chinese":"白平衡"}
    ,{"English":"White Balance (LMS space)","Chinese":"白平衡LMS"}
]

//https://www.mltframework.org/plugins/PluginsFilters/
//https://www.freedesktop.org/software/gstreamer-sdk/data/docs/latest/gst-plugins-bad-plugins-0.10/gst-plugins-bad-plugins-plugin-frei0r.html
var EN_SHORT_list = [
    {"English":"3 point color balance", "Short":"3 Points",      "Description":"Adjust color balance with 3 color points"}
    ,{"English":"3dflippo",             "Short":"3D Rotation",     "Description":"Frame rotation in 3d-space"}
    ,{"English":"B",                    "Short":"B Channel",    "Description":"Extracts Blue from Image"}
    ,{"English":"Baltan",               "Short":"Baltan",       "Description":"delayed alpha smoothed blit of time"}
    ,{"English":"BgSubtract0r",         "Short":"BgSubtract",   "Description":"Perform background subtraction on a still video"}
    ,{"English":"bluescreen0r",         "Short":"Chroma Key",  "Description":"Color to alpha (blit SRCALPHA)"}
    ,{"English":"Brightness",           "Short":"Brightness",   "Description":"Adjusts the brightness of a source image"}
    ,{"English":"bw0r",                 "Short":"Gray",         "Description":"Turns image black/white."}
    ,{"English":"c0rners",              "Short":"Perspective",  "Description":"Perspective transform"}
    ,{"English":"cairogradient",        "Short":"Gradient",     "Description":"Gradient"}
    ,{"English":"cairoimagegrid",       "Short":"ImageGrid",    "Description":"Image grid"}
    ,{"English":"Cartoon",              "Short":"Cartoon",      "Description":"Cartoonify video, do a form of edge detect"}
    ,{"English":"Color Distance",       "Short":"ColorDistance","Description":"Turns image black/white with color distance."}
    ,{"English":"coloradj_RGB",         "Short":"Rgb Adjust",   "Description":"Adjust the red, green, blue channels of a source image"}
    ,{"English":"colorhalftone",        "Short":"Half Tone",    "Description":"Half tone"}
    ,{"English":"colorize",             "Short":"HSB",          "Description":"Adjust the Hue, Saturation, Lightness of a source image"} //hue 值范围可能有错，缺省值应该为0
    ,{"English":"colortap",             "Short":"ColorTap",     "Description":"ColorTap"}
    ,{"English":"Contrast0r",           "Short":"Contrast",     "Description":"Adjusts the contrast of a source image"}
    ,{"English":"Defish0r",             "Short":"Fisheye",      "Description":"Fisheye"}
    ,{"English":"delay0r",              "Short":"Video Delay",  "Description":"Video delay"}  // ???
    ,{"English":"Delaygrab",            "Short":"Delaygrab",    "Description":"Delay grab"}
    ,{"English":"Distort0r",            "Short":"Distort",      "Description":"Plasma"}
    ,{"English":"dither",               "Short":"Dither",       "Description":"Image dithering algorithm"}
    ,{"English":"Edgeglow",             "Short":"EdgeGlow",     "Description":"Edge glow"}
    ,{"English":"emboss",               "Short":"Emboss",       "Description":"Emboss"}
    ,{"English":"Equaliz0r",            "Short":"Equalize",     "Description":"Equalizes the intensity histograms"}
    ,{"English":"Flippo",               "Short":"Flip",         "Description":"Flipping in x and y axis"}
    ,{"English":"G",                    "Short":"G Channel",    "Description":"Extracts Green from Image"}
    ,{"English":"Gamma",                "Short":"Gamma",        "Description":"Adjusts the gamma value of a source image"}
    ,{"English":"Glitch0r",             "Short":"Glitch",        "Description":"Adds glitches and block shifting"}
    ,{"English":"Glow",                 "Short":"Glow",         "Description":"Creates a glamorous glow"}
    ,{"English":"hqdn3d",               "Short":"3D Denoise",        "Description":"High quality 3D denoiser "}
    ,{"English":"Hueshift0r",           "Short":"Hue Shift",        "Description":"Shifts the hue of a source image"}
    ,{"English":"IIR blur",             "Short":"IIR Blur",        "Description":"Three types of fast IIR blurring"}
    ,{"English":"Invert0r",             "Short":"Invert",        "Description":"Inverts all colors of a source image"}
    ,{"English":"K-Means Clustering",   "Short":"K-Means",        "Description":"Clusters of a source image by color and spatial distance"}
    ,{"English":"keyspillm0pup",        "Short":"KeySpill",        "Description":"Reduces the visibility of key color spill in chroma keying"}
    ,{"English":"Lens Correction",      "Short":"Lens Correction",        "Description":"Allows compensation of lens distortion"}
    ,{"English":"LetterB0xed",          "Short":"LetterBox",        "Description":"Adds Black Borders at top and bottom for Cinema Look"}
    ,{"English":"Levels",               "Short":"Levels",        "Description":"Adjust luminance or color channel intensity"}
    ,{"English":"Light Graffiti",       "Short":"Graffiti",        "Description":"Creates light graffitis from a video by keeping the brightest spots."}
    ,{"English":"Luminance",            "Short":"Luminance",        "Description":"Creates a luminance map of the image"}
    ,{"English":"Mask0Mate",            "Short":"Mask",        "Description":"Creates an square alpha-channel mask"}
    ,{"English":"Medians",              "Short":"Medians",        "Description":"Implements several median-type filters"}
    ,{"English":"NDVI filter",          "Short":"NDVI filter",        "Description":"This filter creates a false image from a visible + infrared source."}
    ,{"English":"Nervous",              "Short":"Nervous",        "Description":"flushes frames in time in a nervous way"}
    ,{"English":"Nikon D90 Stairstepping fix","Short":"D90 Fix",        "Description":"Nikon D90 Stairstepping fix"}
    ,{"English":"Normaliz0r",           "Short":"Normalize",        "Description":"Normalize (aka histogram stretch, contrast stretch)"}
    ,{"English":"nosync0r",             "Short":"No Sync",        "Description":"broken tv"}
    ,{"English":"pixeliz0r",            "Short":"Pixelize",        "Description":"Pixelize input image"}
    ,{"English":"posterize",            "Short":"Posterize",        "Description":"Posterizes image by reducing the number of colors used in image"}
    ,{"English":"pr0be",                "Short":"Probe",        "Description":"Measure video values"}
    ,{"English":"pr0file",              "Short":"Profile",        "Description":"2D video oscilloscope"}
    ,{"English":"Premultiply or Unpremultiply","Short":"Premultiply",        "Description":"Multiply (or divide) each color component by the pixel's alpha value"}
    ,{"English":"primaries",            "Short":"Primaries",        "Description":"Reduce image to primary colors"}
    ,{"English":"R",                    "Short":"R Channel",        "Description":""}
    ,{"English":"rgbnoise",             "Short":"Rgb noise",        "Description":"Adds RGB noise to image."}
    ,{"English":"rgbsplit0r",           "Short":"Rgb split",        "Description":"RGB splitting and shifting"}
    ,{"English":"Saturat0r",            "Short":"Saturation",        "Description":"Adjusts the saturation of a source image"}
    ,{"English":"scanline0r",           "Short":"Scanline",        "Description":"interlaced black lines"}
    ,{"English":"select0r",             "Short":"Color Mask",        "Description":"Color based alpha selection"}
    ,{"English":"Sharpness",            "Short":"Sharpness",        "Description":"Unsharp masking "}
    ,{"English":"sigmoidaltransfer",    "Short":"Desaturates", "Description":"Desaturates image and creates a particular look that could be called Stamp, Newspaper or Photocopy"}
    ,{"English":"Sobel",                "Short":"Sobel",        "Description":"Sobel filter"}
    ,{"English":"softglow",             "Short":"Soft Glow",        "Description":"Does softglow effect on highlights"}
    ,{"English":"SOP/Sat",              "Short":"SOP/Sat", "Description":"Slope/Offset/Power and Saturation color corrections according to the ASC CDL (Color Decision List)"}
    ,{"English":"spillsupress",         "Short":"SpillSupress", "Description":"Remove green or blue spill light from subjects shot in front of green or blue screen"}
    ,{"English":"Squareblur",           "Short":"Square Blur",        "Description":"Variable-size square blur"}
    ,{"English":"TehRoxx0r",            "Short":"TV Wall",        "Description":"Something videowall-ish"}
    ,{"English":"threelay0r",           "Short":"ThreeLayor",        "Description":"3 level thresholding"}
    ,{"English":"Threshold0r",          "Short":"Threshold",        "Description":"Thresholds a source image"}
    ,{"English":"Timeout indicator",    "Short":"Timeout",        "Description":"Timeout indicators e.g. for slides."}
    ,{"English":"Tint0r",               "Short":"Tint",        "Description":"Tint a source image with specified color"}
    ,{"English":"Transparency",         "Short":"Transparency",        "Description":"Tunes the alpha channel."}
    ,{"English":"Twolay0r",             "Short":"TwoLayor",        "Description":"dynamic thresholding"}
    ,{"English":"Vertigo",              "Short":"Vertigo",        "Description":"alpha blending with zoomed and rotated images"}
    ,{"English":"Vignette",             "Short":"Vignette",        "Description":"Lens vignetting effect, applies natural vignetting"}
    ,{"English":"White Balance",        "Short":"W Balance",        "Description":"Adjust the white balance / color temperature"}
    ,{"English":"White Balance (LMS space)","Short":"W Balance LMS",        "Description":"Do simple color correction, in a physically meaningful way"}
]


function transEn2Ch(strEn){
    for(var i=0;i<EN_CH_list.length;i++){
        if(strEn == EN_CH_list[i].English){
            strEn = EN_CH_list[i].Chinese
        }
    }
    return strEn
}


function transEn2Short(strEn){
    for(var i=0;i<EN_SHORT_list.length;i++){
        if(strEn == EN_SHORT_list[i].English){
            strEn = EN_SHORT_list[i].Short
        }
    }
    return strEn
}