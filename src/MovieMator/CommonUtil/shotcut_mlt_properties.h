/*
 * Copyright (c) 2013-2016 Meltytech, LLC
 * Author: Dan Dennedy <dan@dennedy.org>
 *
 * Copyright (c) 2016-2019 EffectMatrix Inc.
 * Author: vgawen <gdb_1986@163.com>
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

#ifndef SHOTCUT_MLT_PROPERTIES_H
#define SHOTCUT_MLT_PROPERTIES_H

/* This file contains all of the Shotcut-specific MLT properties.
 * See also https://www.shotcut.org/notes/mltxml-annotations/
 *
 * A property should be prefaced with an underscore if it will not be saved
 * in the XML even if it never has a chance of getting into there. This makes
 * it more clear which is also an XML annotation or purely internal use.
 */

/* MLT XML annotations */

#define kAudioTrackProperty "moviemator:audio"
#define kCommentProperty "moviemator:comment"
#define kShotcutFilterProperty "moviemator:filter"
#define kShotcutPlaylistProperty "moviemator:playlist"
#define kShotcutTransitionProperty "moviemator:transition"
#define kShotcutProducerProperty "moviemator:producer"
#define kShotcutVirtualClip "moviemator:virtual"
#define kTimelineScaleProperty "moviemator:scaleFactor"
#define kTrackHeightProperty "moviemator:trackHeight"
#define kTrackNameProperty "moviemator:name"
#define kTrackLockProperty "moviemator:lock"
#define kVideoTrackProperty "moviemator:video"
#define kShotcutCaptionProperty "moviemator:caption"
#define kShotcutDetailProperty "moviemator:detail"
#define kShotcutHashProperty "moviemator:hash"
#define kFilterTrackProperty "moviemator:filterTrack"

/* Ideally all shotcut properties should begin with "shotcut:", but these
 * do not and kept for legacy reasons? */

#define kAspectRatioNumerator "moviemator_aspect_num"
#define kAspectRatioDenominator "moviemator_aspect_den"
#define kShotcutResourceProperty "moviemator_resource"
#define kShotcutSequenceProperty "moviemator_sequence"

/* Special object Ids expected by Shotcut and used in XML */

#define kBackgroundTrackId "background"
#define kPlaylistTrackId "main bin"

/* Internal only */

#define kAudioLevelsProperty "_moviemator:audio-levels"
#define kBackgroundCaptureProperty "_moviemator:bgcapture"
#define kPlaylistIndexProperty "_moviemator:playlistIndex"
#define kFilterInProperty "_moviemator:filter_in"
#define kFilterOutProperty "_moviemator:filter_out"
#define kThumbnailInProperty "_moviemator:thumbnail-in"
#define kThumbnailOutProperty "_moviemator:thumbnail-out"
#define kUndoIdProperty "_moviemator:undo_id"
#define kUuidProperty "_moviemator:uuid"
#define kMultitrackItemProperty "_moviemator:multitrack-item"

#endif // SHOTCUT_MLT_PROPERTIES_H
