/*
 * Copyright (c) 2013-2015 Meltytech, LLC
 * Author: Dan Dennedy <dan@dennedy.org>
 *
 * Copyright (c) 2016-2019 EffectMatrix Inc.
 * Author: vgawen <gdb_1986@163.com>
 * Author: WanYuanCN <ebthon@hotmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

function scrollIfNeeded() {
    console.assert(timeline);
    console.assert(scrollView);
    if(!timeline) return;
    var x = timeline.position * multitrack.scaleFactor;
    if (!scrollView) return;
    if (x > scrollView.flickableItem.contentX + scrollView.width - 50)
        // scrollView.flickableItem.contentX = x - scrollView.width + 50;
        // 刻度多的话播放时刷新不会卡
        scrollView.flickableItem.contentX = x - scrollView.width + 500;
    else if (x < 50)
        scrollView.flickableItem.contentX = 0;
    else if (x < scrollView.flickableItem.contentX + 50)
        scrollView.flickableItem.contentX = x - 50;    
}

function scrollIfZoomNeeded(wheelx, scaleValue) {
    // scrollView以鼠标为中心向两边缩放，使scrollView的左边界与鼠标位置的距离按比例缩放
    // wheelx：鼠标（滚轮）位置
    // scaleValue：缩放前的缩放系数
    console.assert(scrollView);
    if (!scrollView) return;
    scrollView.flickableItem.contentX = (wheelx+scrollView.flickableItem.contentX)*multitrack.scaleFactor/scaleValue - wheelx;

    // 如果轨道超出屏幕范围，就直接定位到屏幕右侧
    // 以轨道长度中曾经最大的计算
    toolbar.maxWidth = toolbar.maxWidth > tracksContainer.width ? toolbar.maxWidth : tracksContainer.width;
    if(scrollView.flickableItem.contentX > toolbar.maxWidth){
        scrollView.flickableItem.contentX = toolbar.maxWidth - scrollView.width;
    }
    // 轨道实际长度小于 scrollView的长度时特殊处理下
    if((scrollView.width > tracksContainer.width/multitrack.scaleFactor*1.01) && (scrollView.flickableItem.contentX > toolbar.maxWidth-scrollView.width)){
        scrollView.flickableItem.contentX = toolbar.maxWidth - scrollView.width;
    }
    // 不能小于 0
    if(scrollView.flickableItem.contentX < 0){
        scrollView.flickableItem.contentX = 0;
    }
}

function dragging(pos, duration) {
    console.assert(tracksRepeater);
    console.assert(scrollView);
    console.assert(timeline);
    if(!tracksRepeater || !scrollView || !timeline) return;
    if (tracksRepeater.count > 0) {
        var headerHeight = ruler.height + toolbar.height
        dropTarget.x = pos.x
        dropTarget.width = duration * multitrack.scaleFactor
        
        for (var i = 0; i < tracksRepeater.count; i++) {
            console.assert(tracksRepeater.itemAt(i));
            if(!tracksRepeater.itemAt(i)) continue;
            var trackY = tracksRepeater.itemAt(i).y + headerHeight - scrollView.flickableItem.contentY
            var trackH = tracksRepeater.itemAt(i).height
            if (pos.y >= trackY && pos.y < trackY + trackH) {
                currentTrack = i
                if (pos.x > headerWidth) {
                    dropTarget.height = trackH
                    dropTarget.y = trackY
                    if (dropTarget.y < headerHeight) {
                        dropTarget.height -= headerHeight - dropTarget.y
                        dropTarget.y = headerHeight
                    }
                    dropTarget.visible = true
                }
                break
            }
        }
        if (i === tracksRepeater.count || pos.x <= headerWidth)
            dropTarget.visible = false

        // Scroll tracks if at edges.
        if (pos.x > headerWidth + scrollView.width - 50) {
            scrollTimer.backwards = false
            scrollTimer.start()
        } else if (pos.x >= headerWidth && pos.x < headerWidth + 50) {
            if (scrollView.flickableItem.contentX < 50) {
                scrollView.flickableItem.contentX = 0;
                scrollTimer.stop()
            } else {
                scrollTimer.backwards = true
                scrollTimer.start()
            }
        } else {
            scrollTimer.stop()
        }

        if (toolbar.scrub) {
            timeline.position = Math.round(
                (pos.x + scrollView.flickableItem.contentX - headerWidth) / multitrack.scaleFactor)
        }
        if (toolbar.snap) {
            for (i = 0; i < tracksRepeater.count; i++)
                tracksRepeater.itemAt(i).snapDrop(pos)
        }
    }
}

function dropped() {
    dropTarget.visible = false
    scrollTimer.running = false
}

function acceptDrop(xml) {
    console.assert(scrollView);
    if(!scrollView) return;
    var position = Math.round((dropTarget.x + scrollView.flickableItem.contentX - headerWidth) / multitrack.scaleFactor)
//    if (toolbar.ripple)
    console.assert(timeline);
    if(timeline)
        timeline.insert(currentTrack, position, xml)
//    else
//        timeline.overwrite(currentTrack, position, xml)
}

function acceptDropUrls(urls)
{
    console.assert(timeline);
    if(timeline)
        timeline.appendFromUrls(currentTrack, urls);
}

function acceptDropListItem(items)
{
    console.assert(timeline);
    if(timeline)
        timeline.appendFromAbstractModelItemDataList(currentTrack, items);
}

function trackHeight(isVideo) {
//    return isAudio? Math.max(40, multitrack.trackHeight) : multitrack.trackHeight * 2
//    return isVideo? multitrack.trackHeight * 2 : Math.max(30, multitrack.trackHeight);
    return isVideo? 50 : Math.max(30, multitrack.trackHeight);
}
