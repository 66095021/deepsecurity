#!/usr/bin/env python
import json
#sbx_ip_list=[line.strip() for line in open ('/opt/sbx/sbxserver.config', 'r')]


f=file('/opt/ATI/analyzer.config', 'r')
jsonobj=json.load(f)
f.close()
file_size_max=jsonobj["file_size_max"]
file_trunk_dir=jsonobj["file_trunk_dir"]
file_meta_info_dir=jsonobj["file_meta_info_dir"]
cloud_server=jsonobj["cloud_server"]

if __name__== '__main__':

    print file_meta_info_dir,  file_trunk_dir, file_size_max, cloud_server
