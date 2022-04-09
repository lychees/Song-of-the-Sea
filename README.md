# 简介
Yet another rpg game.

- [Demo](https://yno.yumenikki.info/?game=demo)
- [Github](https://github.com/lychees/Song-of-the-Sea)
- [背景设定](https://hackmd.io/vZ8oGrSMTACMHRaiow4F2w)
- [卡表](https://docs.google.com/spreadsheets/d/1Rn8yd1afu2ArHv9ayAUR_0suPwzvrUx617NXJwWf9J4/edit?ouid=102027559639806204339&usp=sheets_home&ths=true)

# 如何编译
## Windows
具体参考这里 [https://github.com/EasyRPG/Player/issues/2758](https://github.com/EasyRPG/Player/issues/2758)，Windows 下的 cpp 包管理工具主要使用 vcpkg，其中 liblcf 会自动编译，因此只需要准备 [buildscripts](https://github.com/EasyRPG/buildscripts) 即可。

```cmake . -DPLAYER_BUILD_LIBLCF=ON -DCMAKE_BUILD_TYPE=Debug -DVCPKG_TARGET_TRIPLET=x64-windows-static -DCMAKE_TOOLCHAIN_FILE=C:\Users\25181\Documents\GitHub\buildscripts\windows\vcpkg\scripts\buildsystems\vcpkg.cmake```

```cmake --build .```
## Web
参考这里 [https://easyrpg.org/player/guide/webplayer/](https://easyrpg.org/player/guide/webplayer/)。

### 部署
```scp ubuntu@54.168.220.93:~/workdir/Song-of-the-Sea/Player/build/index.js ubuntu@54.168.220.93:~/workdir/Song-of-the-Sea/Player/build/index.wasm root@45.55.22.97:/srv/http/yumenikki/yno/private/public/```

# To do list
## 系统
- [x] 固定每局游戏里随机地牢的种子，使得每次访问地图都得到同一张
- [ ] 地牢种子可在 ini 文件中编辑（？）
- [ ] 更加智能的聊天框
- [x] FOV 可以显示完整墙壁
- [ ] FOV 可以显示部分格子（类似 [这里](https://rpg.blue/thread-408337-1-1.html) 和 [这里](https://rpg.blue/thread-480426-1-1.html)）
- [ ] FOV 不显示视野外的 map_event
- [ ] 小地图
- [ ] 地图名字

## 剧本
### 序章
#### 幕 1
- [x] 更加科学的开场镜头
- [x] 今天的风有点喧嚣，需要风的音效
- [x] 小动物们做鸟兽散需要动物们的音效，鸡叫应该可以从 Harvest Moon 里蒯 ...
- [ ] 消灭掉先遣部队之后应该还有一波大部队，需要到地图上寻找并逐个消灭，直到被制止
#### 幕 3
- [x] 地牢用战斗背景
- [ ] 应该有一个鸟类来递钥匙🔑
- [ ] 睡着之后黑屏的这段时间，是另外切一个屏幕介绍世界观的好时机。。。

#### 幕 6
- [ ] 船上的独白

### 章 2
- [ ] 扩展菜单

## 素材
- https://forums.rpgmakerweb.com/index.php?threads/grannys-lists-vx-ace-animal-sprites.30456/
- https://grandmadebslittlebits.wordpress.com/2015/01/17/animal-sprites-worth-keeping-resurrected-from-dead-sites/
- http://www.rpg-maker.fr/ressources/charsets/animaux/
