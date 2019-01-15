#coding:utf-8
#TODO：细节需要改进，比如对Operation传参数
#TODO: 把执行的随机操作及参数写到文件，可以读文件重复执行测试步骤

import os
import time
import random
from abc import ABCMeta, abstractmethod  

# '/Users/gdb/Desktop/video/video1'
# 文件路径，每个路径对应一个视频轨道
videoPaths = ('/Users/gdb/Desktop/video/20160125/', '/Users/gdb/Desktop/video/video1')
audioPaths = ('/Users/gdb/Desktop/video/audio',)


def get_random_track():
	tracksNum = maincontroller.trackNums()
	trackIndex = -1
	if (tracksNum > 0) :
		trackIndex = random.randint(0, tracksNum - 1)
	return trackIndex


def get_random_clip(trackIndex):
	clipCount = maincontroller.clipsNum(trackIndex)
	clipIndex = -1
	if (clipCount > 0) :
		clipIndex = random.randint(0, clipCount  - 1)
	return clipIndex


class Operation():
	__metaclass__ = ABCMeta

	def __init__(self):
		pass
	
	@abstractmethod
	def execute(self):
		pass


class UndoOperation(Operation):
	def __init__(self):
		pass
	def execute(self):
		print self.__class__
		maincontroller.undo()

class RedoOperation(Operation):
	def __init__(self):
		pass
	def execute(self):
		print self.__class__
		maincontroller.redo()

class LiftOperation(Operation):
	def __init__(self):
		pass
	def execute(self):
		print self.__class__
		trackIndex = get_random_track()
		clipIndex = get_random_clip(trackIndex)
		timeline.lift(trackIndex, clipIndex)

class RemoveOperation(Operation):
	def __init__(self):
		pass
	def execute(self):
		print self.__class__
		trackIndex = get_random_track()
		clipIndex = get_random_clip(trackIndex)
		timeline.remove(trackIndex, clipIndex)


class SplitOperation(Operation):
	def __init__(self):
		pass
	def execute(self):
		print self.__class__
		trackIndex = get_random_track()
		clipIndex = get_random_clip(trackIndex)
		position = maincontroller.getStartPositionOfClip(trackIndex, clipIndex)
		maincontroller.splitClip(trackIndex,clipIndex,position + 50)



class TrimInOperation(Operation):
	def __init__(self):
		pass
	def execute(self):
		print self.__class__
		trackIndex = get_random_track()
		clipIndex = get_random_clip(trackIndex)
		timeline.trimClipIn(trackIndex, clipIndex, 20, 0)


class TrimOutOperation(Operation):
	def __init__(self):
		pass
	def execute(self):
		print self.__class__
		trackIndex = get_random_track()
		clipIndex = get_random_clip(trackIndex)
		timeline.trimClipOut(trackIndex, clipIndex, 20, 0)

class MoveOperation(Operation):
	def __init__(self):
		pass
	def execute(self):
		print self.__class__
		fromTrackIndex = get_random_track()
		toTrackIndex = get_random_track()
		clipIndex = get_random_clip(fromTrackIndex)
		timeline.moveClip(fromTrackIndex, toTrackIndex, clipIndex, 0)

class AddTransitionOperation(Operation):
	def __init__(self):
		pass
	def execute(self):
		print self.__class__
		trackIndex = get_random_track()
		clipIndex = get_random_clip(trackIndex)
		maincontroller.addTransition(trackIndex, clipIndex, 1.0)

class SetTransitionTypeOperation(Operation):
	def __init__(self):
		pass
	def execute(self):
		print self.__class__
		trackIndex = get_random_track()
		clipIndex = get_random_clip(trackIndex)
		#获取的Clip不一定是transition
		#transition 类型应该随机
		maincontroller.setTransitionType(trackIndex, clipIndex, 5)

class SetTransitionDurationOperation(Operation):
	def __init__(self):
		pass
	def execute(self):
		print self.__class__
		trackIndex = get_random_track()
		clipIndex = get_random_clip(trackIndex)
		#获取的Clip不一定是transition
		maincontroller.setTransitionDuration(trackIndex,clipIndex,50)

class RemoveTransitionOperation(Operation):
	def __init__(self):
		pass
	def execute(self):
		print self.__class__
		trackIndex = get_random_track()
		clipIndex = get_random_clip(trackIndex)
		#获取的Clip不一定是transition
		timeline.removeTransition(trackIndex, clipIndex)

class SetSpeedOperation(Operation):
	def __init__(self):
		pass
	def execute(self):
		print self.__class__
		trackIndex = get_random_track()
		clipIndex = get_random_clip(trackIndex)
		maincontroller.setClipSpeed(trackIndex, clipIndex, 2.0)

class AddFilterOperation(Operation):
	def __init__(self):
		pass
	def execute(self):
		print self.__class__
		trackIndex = get_random_track()
		clipIndex = get_random_clip(trackIndex)
		# 需要一个所有filter的列表
		maincontroller.addFilter(trackIndex, clipIndex, "dynamicText")

class RemoveFilterOperation(Operation):
	def __init__(self):
		pass
	def execute(self):
		print self.__class__
		trackIndex = get_random_track()
		clipIndex = get_random_clip(trackIndex)
		# 需要一个所有filter的列表
		maincontroller.removeFilter(trackIndex, clipIndex, 0)



class CopyOperation(Operation):
	def __init__(self):
		pass
	def execute(self):
		print self.__class__
		trackIndex = get_random_track()
		clipIndex = get_random_clip(trackIndex)
		maincontroller.copyClip(trackIndex,clipIndex)


class PasteOperation(Operation):
	def __init__(self):
		pass
	def execute(self):
		print self.__class__
		trackIndex = get_random_track()
		maincontroller.pasteClip(trackIndex, 200)


class CutOperation(Operation):
	def __init__(self):
		pass
	def execute(self):
		print self.__class__
		trackIndex = get_random_track()
		clipIndex = get_random_clip(trackIndex)
		maincontroller.cutClip(trackIndex,clipIndex)

class PlayOperation(Operation):
	def __init__(self):
		pass
	def execute(self):
		print self.__class__
		player.play(1.0)

class SaveOperation(Operation):
	def __init__(self):
		pass
	def execute(self):
		print self.__class__
		mainwindow.saveXML('/Users/gdb/Desktop/000.mmp')


class ExportOperation(Operation):
	def __init__(self):
		pass
	def execute(self):
		print self.__class__
		maincontroller.exportFile('/Users/gdb/Desktop/000.mp4')


class AddVideoTrackOperation(Operation):
	def __init__(self):
		pass
	def execute(self):
		print self.__class__
		timeline.addVideoTrack()

class AddAudioTrackOperation(Operation):
	def __init__(self):
		pass
	def execute(self):
		print self.__class__
		timeline.addAudioTrack()

class InsertTrackOperation(Operation):
	def __init__(self):
		pass
	def execute(self):
		print self.__class__
		trackIndex = get_random_track()
		maincontroller.insertTrack(trackIndex)

class RemoveTrackOperation(Operation):
	def __init__(self):
		pass
	def execute(self):
		print self.__class__
		trackIndex = get_random_track()
		maincontroller.removeTrack(trackIndex)

class LockTrackOperation(Operation):
	def __init__(self):
		pass
	def execute(self):
		print self.__class__
		trackIndex = get_random_track()
		timeline.setTrackLock(trackIndex, 1)


class HideTrackOperation(Operation):
	def __init__(self):
		pass
	def execute(self):
		print self.__class__
		trackIndex = get_random_track()
		timeline.toggleTrackHidden(trackIndex)		

class MuteTrackOperation(Operation):
	def __init__(self):
		pass
	def execute(self):
		print self.__class__
		trackIndex = get_random_track()
		timeline.toggleTrackMute(trackIndex)


operations = (UndoOperation, RedoOperation, LiftOperation, RemoveOperation, 
	SplitOperation, TrimInOperation, TrimOutOperation, MoveOperation,
	RemoveTransitionOperation,
	SetSpeedOperation,
	AddFilterOperation, RemoveFilterOperation)




# def get_random_operation():
# 	operationsCount = len(operations)
# 	operationIndex = random.randint(0, operationsCount - 1)
# 	operation = operations[operationIndex]
# 	return operation


# 执行操作，并记录
# def execute_a_operation(operation):




#获取文件列表
def get_file_list(path):
	files = []
	for (dirpath, dirnames, filenames) in os.walk(path):
		for afile in filenames:
			files.append(os.path.join(dirpath, afile))
	return files

#添加文件到轨道
#add_transition 是否在相邻的clip之间添加transition
def append_files_to_track(trackIndex, fileList, addTransition):
	if (addTransition):
		for afile in fileList:
			clipIndex = maincontroller.appendToTimelineFromPath(trackIndex, afile)
			maincontroller.addTransition(trackIndex, clipIndex, -1.0)
	else:
		maincontroller.appendToTimelineTrack(trackIndex, fileList)




def main():


	# 导入资源到时间线
	videoPathsNum = len(videoPaths)
	videoTracksNum = maincontroller.videoTrackNums()
	audioPathsNum = len(audioPaths)
	audioTracksNum = maincontroller.audioTrackNums()

	while (videoPathsNum > videoTracksNum):
		timeline.addVideoTrack()
		videoTracksNum = maincontroller.videoTrackNums()
	while (audioPathsNum > audioTracksNum):
		timeline.addAudioTrack()
		audioTracksNum = maincontroller.audioTrackNums()

	for index, path in enumerate(videoPaths) :
		fileList = get_file_list(path)
		append_files_to_track(index, fileList, 0)

	tracksNum = maincontroller.trackNums()
	for index, path in enumerate(audioPaths) :
		fileList = get_file_list(path)
		append_files_to_track(tracksNum - videoTracksNum + index, fileList, 0)
	

	# # 对时间线的操作
	# times = 100
	# totalOperationCount = len(operations)
	# for i in range(times):
	# 	opIndex = random.randint(0, totalOperationCount - 1)
	# 	op = operations[opIndex]
	# 	op().execute()

	for i in range(100):
		op = AddFilterOperation()
		op.execute()

	#播放
	# player.play(1.0)

	#保存工程
	# mainwindow.saveXML('/Users/gdb/Desktop/000.mmp')

	#导出视频
	# maincontroller.exportFile('/Users/gdb/Desktop/000.mp4')

main()




# 打开文件，添加到播放列表
# mainwindow.openFiles(filelist)

#把文件直接加入时间线轨道
# maincontroller.appendToTimelineTrack(1, filelist)
# timeline.appendFromPath(0, '/Users/gdb/Desktop/exercise---.mp4')

# 添加轨道
# timeline.addVideoTrack()
# timeline.addAudioTrack()

# 参数：1 索引
# timeline.toggleTrackMute(1)

# 参数：1 索引
# timeline.toggleTrackHidden(1)

# 参数：1 轨道索引，2 lock标记
# timeline.setTrackLock(1, 1)

#插入轨道，参数：1 索引
# maincontroller.insertTrack(1)
#删除轨道，参数：1 索引
# maincontroller.removeTrack(1)

#删除片段，参数：1 轨道索引，2 片段索引
# timeline.remove(1,1)
# timeline.lift(1,0)

#分割片段 参数：1 轨道索引，2 片段索引，3 位置
# position = maincontroller.getStartPositionOfClip(1, 1)
# maincontroller.splitClip(1,1,position + 50)

#trim，参数：1 轨道索引，2 片段索引 3 帧数 4 级联选项
# timeline.trimClipIn(1,0,20,0)
# timeline.trimClipOut(1,1,20,0)

#移动clip, moveClip(int fromTrack, int toTrack, int clipIndex, int position)
# timeline.moveClip(1,0,1,0)


#添加转场，参数：1 轨道索引，2 片段索引，3 转场时间
# maincontroller.addTransition(1, 0, 1.0)

#修改转场类型，，参数：1 轨道索引，2 片段索引，3 转场索引
# maincontroller.setTransitionType(1, 2, 5)

#修改转场时间，参数：1 轨道索引，2 片段索引，3 帧数
# maincontroller.setTransitionDuration(1,2,50)

#删除转场，参数：1 轨道索引，2 片段索引
# timeline.removeTransition(1, 2)


#设置片段速度，参数：1 轨道索引，2 片段索引，3 速度
# maincontroller.setClipSpeed(1, 0, 2.0)

#添加滤镜，参数：1 轨道索引，2 片段索引，3 滤镜id
# maincontroller.addFilter(1, 0, "dynamicText")

#删除滤镜，参数：1 轨道索引，2 片段索引，3 滤镜索引
# maincontroller.removeFilter(1, 0, 0)

#复制，参数：1 轨道，2 片段
# maincontroller.copyClip(1,0)

#剪切，参数：1 轨道，2 片段
# maincontroller.cutClip(1,1)

#粘贴，参数：1 轨道，2 位置
# maincontroller.pasteClip(0, 200)

# 工程设置
# maincontroller.setProfile('dv_ntsc_wide')

#
# maincontroller.undo()
# maincontroller.redo()
# 

#播放,参数：播放速度
# player.play(1.0)

#保存工程
# mainwindow.saveXML('/Users/gdb/Desktop/000.mmp')

#导出视频
# maincontroller.exportFile('/Users/gdb/Desktop/000.mp4')


