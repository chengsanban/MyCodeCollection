#!/bin/bash
echo “进入脚本目录”
cd `dirname $0`
echo "进入静态页面目录....."
cd ../../chengsanban.github.io
echo "删除静态网站文件"
#shopt -u extglob #关闭排除模式
shopt -s extglob #打开排除模式
rm -rf !(image)
echo "进入hugo网站目录"
cd ../StudyNote/hugofile/blog
echo "删除public目录文件....."
rm -rf public/*
echo "生成新网站静态文件....."
hugo
echo "进入git博客目录....."
cd ../../../chengsanban.github.io
echo "拷贝public内容到当前目录....."
cp -r ../StudyNote/hugofile/blog/public/* .
echo "删除生成的原始文件"
rm -rf ../StudyNote/hugofile/blog/public/*
echo "开始提交....."
git add *
git commit -am "new article"
git push origin master
