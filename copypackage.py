import zipfile
import os
folders = ["src","examples"]
files = ["library.properties"]
exclude_list = ["myconfig.h"]
library_dir = os.path.dirname(__file__)
zip_root_dir = "BLiveDM"
def zipfile_write_folder(zfile,src_path,zpath):
    for path, dirnames, filenames in os.walk(src_path):  # 遍历文件
        target_path = path.replace(src_path, zpath)  # 去掉目标跟路径，只对目标文件夹下边的文件及文件夹进行压缩（即生成相对路径）
        for filename in filenames:
            if filename in exclude_list:
                continue
            zfile.write(os.path.join(path, filename), os.path.join(target_path, filename))
with zipfile.ZipFile(os.path.join(library_dir,'bilidm.zip'), 'w') as zipObj:
    # 压缩文件夹
    for folder in folders:
        zipfile_write_folder(zipObj,os.path.join(library_dir,folder),os.path.join(zip_root_dir,folder))
    for file in files:
        zipObj.write(os.path.join(library_dir,file),os.path.join(zip_root_dir,file))