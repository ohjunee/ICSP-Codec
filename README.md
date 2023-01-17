# Codec Description 


## oj-codec-v2: 
_ICSP CODEC 2.4 :_ [_paper_](https://drive.google.com/file/d/1FbLcp5k9eHsrOxJReUN2g5NRViy-FmTo/view?usp=share_link)  
## Introduction
> I am sure that this document can help your ICSP Codec design. Main purpose of this document is descriptions for encoder and decoder. First of all, read suggestions in the next page, if you think that you need more study or understanding and then try after studying that. we just deal with 4:2:0 formatted sequence for encoder and decoder descriptions. 4:2:2 and 4:4:4 formats can be generated by 4:2:0 format sequence using interpolation.

## General description for ICSP codec design

> In this document, we just deal with 4:2:0 formatted sequence for encoder and decoder descriptions. 4:2:2 and 4:4:4 formats can be generated by 4:2:0 format sequence using interpolation.
> 
> 

### Image padding  
> ![fig1](https://user-images.githubusercontent.com/49416429/209584035-2c0feacb-e118-4244-ae5c-127fe1f789db.png)

> ### Block coding order
> Fig 2. shows coding order of 4:2:0 format. As you can see, large character numbers mean macro-block order (raster scan order) and small character numbers mean real coding order after transform. In case of 4:2:0 format, sizes of Cb and Cr are half of luma.  
> 
> ![스크린샷_20221227_062612](https://user-images.githubusercontent.com/49416429/209584150-6f19572f-a9eb-4729-b8b6-1548e5486588.png)  
> Fig. 2. Coding order for 4:2:0 format.



## Parameter

> In this section, we define some parameters such as size, quantization parameter, and some modes.
> 
> Size 352x288 (CIF),
> DPCM modes for DC prediction and pixel prediction
> -	Mode 0 : Median prediction mode
> -	Mode 6 : no prediction
> Intra prediction flag
> -	Mode 0/1 : Disable / Enable
> Intra period for frame type
> -	Range : 0 ~ 10, 0 means all predicted frame except first frame.
> Quantization Parameter
> -	For DC, 1 ~ 16
> -	For AC, 1 ~ 16



## Encoder description

> The ICSP encoder structure. T and Q mean transform and quantization each. We use simplified intra 8x8 prediction, Discrete Cosine Transform (DCT) as T and uniform quantization with simple dead zone as Q. Zig-zag scan is used for reordering and entropy coder which is like JPEG’s is used. For inter frame, Motion Estimation (ME) and Motion Compensation (MC) are used. Reconstructed frame has to be made using inverse transform and inverse quantization. 
> 
> ![스크린샷_20221227_064038](https://user-images.githubusercontent.com/49416429/209584770-42935577-e032-4cb0-a655-d0a88c1e9987.png)
> Fig. 3. Block diagram of ICSP encoder.
> 
> T : transform  DCT  
> Q : quantization  general uniform quantization with simple dead zone  
> Reorder  zig-zag scan  
> Entropy encoder  like JPEG  



### Encoding a intra frame

> For encoding the intra frame, green units in Fig. 4 are used.  
> 
> ![스크린샷_20221227_064118](https://user-images.githubusercontent.com/49416429/209584798-dfdd32df-e69f-4bb8-99ba-36a7d89460b8.png)
> Fig. 4. Block diagram of intra frame.  
> 
> In case of I frame, first of all, current frame is divided by macro-block. In raster macro-block scan order, after simplified intra 8x8 prediction, pixel based DPCM, discrete cosine transform, and quantization, we also use DPCM for reconstructed (i.e, quantized and inverse quantized) DC component. And then use the reordering (just zig-zag scan) and entropy encoder (like Huffman coding). While doing this forward process, we save the reconstructed data using inverse quantization, inverse transform, inverse DPCM and simplified intra 8x8 prediction for reference of next frame.



### Most Probable Mode

> ![image](https://user-images.githubusercontent.com/49416429/210853033-5c1ea16e-909d-429b-806a-9d362f611063.png) 
> 
> To reduce the mode bit for intra mode, we use prediction using neighboring blocks. If median of Upper, Left, and Upper Left is equal to current mode, MPM_flag will be set to one. If not, MPM_flag will be set to zero and additional fixed mode bit is coded.



### Encoding a inter frame

> For encoding the inter frame, green units in Fig. 5 are used.
> 
> ![스크린샷_20221227_063259](https://user-images.githubusercontent.com/49416429/209584395-ce7c3b29-b144-4f51-bfb5-989e7e43ec14.png)  
> Fig. 5. Block diagram of inter frame.  
> 
> In case of P frame, using previous reconstructed frame for reference, we use motion estimation to search the best motion vector of current macro-block.  After motion estimation, we use DCT, quantization, DPCM for DC, reordering, and entropy coding for error image. We use motion compensation for reconstructed frame. In the inter prediction, we do not use pixel based DPCM.

### Decoder description
> Generally, encoder contains decoder. Therefore decoder is very simple. 
> Fig. 12 shows the structure of ICSP decoder.
> As you know, Fig. 3 contains Fig. 12.
> 
> ![스크린샷_20221227_062159](https://user-images.githubusercontent.com/49416429/209583972-08a8eaf2-93fd-47ef-9f71-554b40e086a7.png)  
> Fig. 12. Block diagram of ICSP decoder.

## Result image for each module  

### Intra prediction only  
> 
> ![스크린샷_20221227_064814](https://user-images.githubusercontent.com/49416429/209585226-a09d2355-4115-4239-8c9d-bcbbcb0a615c.png)  

### Intra + Pixel DPCM  
> 
> ![스크린샷_20221227_065044](https://user-images.githubusercontent.com/49416429/209585316-1e63e0c6-f29c-453b-a2c7-35923a1a360b.png)  

### Inter prediction (Motion vector)  
> 
> ![스크린샷_20221227_065052](https://user-images.githubusercontent.com/49416429/209585371-912af8e8-3175-4155-84a7-63092ef665c4.png)  

### DCT Only  
> 
> ![스크린샷_20221227_065059](https://user-images.githubusercontent.com/49416429/209585375-77d2bb67-62de-4a6f-b38f-7ae49b14a815.png)  

### Only Quantization(QP=16)  
>
> ![스크린샷_20221227_065106](https://user-images.githubusercontent.com/49416429/209585289-c7e918f7-7da2-4b52-bf51-029a4f0cb094.png)  

### DC DPCM Only  
>
> ![스크린샷_20221227_065113](https://user-images.githubusercontent.com/49416429/209585299-2fb4fbbd-1678-45f7-811f-001384c41d77.png)  

### Reorder Only  
> 
> ![스크린샷_20221227_065119](https://user-images.githubusercontent.com/49416429/209585305-9baa4158-9726-47a7-b4d8-2f293eb2a601.png)  


## 2. oj-codec-v1
![스크린샷_20221227_051214](https://user-images.githubusercontent.com/49416429/209580732-beed568c-662d-4a9d-b162-03774960e71d.png)
![스크린샷_20221227_051356](https://user-images.githubusercontent.com/49416429/209580780-b085608f-45f1-4af9-addc-7744689aaca9.png)


## 3. FastME-practice  
> On the Fast Motion Estimation Algorithm for Inter Prediction.  
>
> MPEG and H.264 standards use motion estimation-based inter prediction, which removes temporal redundancy between frames for image compression. 
> 
> Based on Block matching algorithm that finds the most similar regions in the frame, various methods for psnr and fast algorithm have been proposed.
>   
> This full search technique, the most classic block-based motion estimation method for video encoding, compares SAD values at all locations within the search area of the previous frame. Therefore, the exact Motion Vector can be found, but computational cost is much higher.  
>   
> This project aims to find out the difference between the two fast block matching algorithms, PDE, which is a lossless ME, and PDS, which is a lossy ME, compared to Full Search Algorithm.


### Example
> https://user-images.githubusercontent.com/49416429/210852230-06a8e5c8-105a-47d3-b7a9-034c62606aa4.mp4
