# fileWatcher

- linux下的文件更改监视器

- 能够监视文本文件被是否被保存

- 功能基于linux下的inotify系统调用

- 不同编辑器的保存策略不同，可能会有不同的表现

  1. vsocde采用直接保存，测试不会有问题
  2. vim采用覆盖保存，测试不会有问题
  3. gedit采用多次保存，测试一次保存会调用二次回调
  4. nano会在启动时进行一次保存，其他没有问题
  5. mv，cp命令可以正常检测
  6. emacs暂时不支持

  其他编辑器理论上也可以支持，建议使用测试程序先进行测试