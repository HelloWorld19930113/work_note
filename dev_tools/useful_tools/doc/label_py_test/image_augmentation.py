#coding=utf-8
'''
代码：图片数据增加小工具

按照前面4个部分的思路和方法，这节来实现这么一个图片数据增加小工具，首先对于一些基础的操作，我们定义在一个叫做image_augmentation.py的文件里：
'''

# 每个进程内调用图像处理函数进行扰动的函数
def augment_images(filelist, args):
    # 遍历所有列表内的文件
    for filepath, n in filelist:
        img = cv2.imread(filepath)
        filename = filepath.split(os.sep)[-1]
        dot_pos = filename.rfind('.')
		
		# 获取文件名和后缀名
        imgname = filename[:dot_pos]
        ext = filename[dot_pos:]

        print('Augmenting {} ...'.format(filename))
        for i in range(n):
            img_varied = img.copy()
			
			# 扰动后文件名的前缀
            varied_imgname = '{}_{:0>3d}_'.format(imgname, i)
			
			# 按照比例随机对图像进行镜像
            if random.random() < args.p_mirror:
			    # 利用numpy.fliplr(img_varied)也能实现
                img_varied = cv2.flip(img_varied, 1)
                varied_imgname += 'm'
			
			# 按照比例随机对图像进行裁剪
            if random.random() < args.p_crop:
                img_varied = ia.random_crop(
                    img_varied,
                    args.crop_size,
                    args.crop_hw_vari)
                varied_imgname += 'c'
			
			# 按照比例随机对图像进行旋转
            if random.random() < args.p_rotate:
                img_varied = ia.random_rotate(
                    img_varied,
                    args.rotate_angle_vari,
                    args.p_rotate_crop)
                varied_imgname += 'r'
			
			# 按照比例随机对图像进行HSV扰动
            if random.random() < args.p_hsv:
                img_varied = ia.random_hsv_transform(
                    img_varied,
                    args.hue_vari,
                    args.sat_vari,
                    args.val_vari)
                varied_imgname += 'h'
			
			# 按照比例随机对图像进行Gamma扰动
            if random.random() < args.p_gamma:
                img_varied = ia.random_gamma_transform(
                    img_varied,
                    args.gamma_vari)
                varied_imgname += 'g'
			
			# 生成扰动后的文件名并保存在指定的路径
            output_filepath = os.sep.join([
                args.output_dir,
                '{}{}'.format(varied_imgname, ext)])
            cv2.imwrite(output_filepath, img_varied)

