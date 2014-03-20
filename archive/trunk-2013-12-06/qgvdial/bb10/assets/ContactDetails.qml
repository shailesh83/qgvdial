/*
 qgvdial is a cross platform Google Voice Dialer
 Copyright (C) 2009-2013  Yuvraaj Kelkar
 
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 
 Contact: yuvraaj@gmail.com
 */

import bb.cascades 1.0
import com.kdab.components 1.0

Page {
    id: container
    
    property alias contactImageSource: contactImage.imageSource
    property alias contactNameText: contactName.text
    
    attachedObjects: [
        AbstractItemModel {
            id: contactsPhonesModel
            
            sourceModel: g_ContactPhonesModel
        },
        TextStyleDefinition {
            id: tsdRight
            base: SystemDefaults.TextStyles.BodyText
            textAlign: TextAlign.Right
        },
        TextStyleDefinition {
            id: tsdxlarge
            base: tsdRight.style
            fontSize: FontSize.XXLarge
        }
    ]

    Container {
        layout: StackLayout { orientation: LayoutOrientation.TopToBottom }
        
        Container {
            layout: StackLayout { orientation: LayoutOrientation.LeftToRight }
            layoutProperties: StackLayoutProperties { spaceQuota: 1 }
            
            ImageView {
                id: contactImage
                preferredHeight: 140
                preferredWidth: 140
                verticalAlignment: VerticalAlignment.Center
            }
            
            Label {
                id: contactName
                verticalAlignment: VerticalAlignment.Center

                textStyle { base: tsdxlarge.style }
            }//Label: contact name
        }
        
        Container {
            ListView {
                dataModel: contactsPhonesModel
                
                listItemComponents: [
                    ListItemComponent {
                        Container {
                            id:  listItem
                            
                            layout: StackLayout {
                                orientation: LayoutOrientation.TopToBottom
                            }

                            Container {
                                //layout: DockLayout { }
                                layout: StackLayout {
                                    orientation: LayoutOrientation.LeftToRight
                                }
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 1
                                }
                                
                                Label {
                                    text: ListItemData.type
                                }
                                
                                Label {
                                    text: ListItemData.number
                                    textStyle {
                                        base: SystemDefaults.TextStyles.BodyText
                                        textAlign: TextAlign.Right
                                    }
                                }
                            }//Row of type and number
                            
                            Divider {
                                horizontalAlignment: HorizontalAlignment.Fill
                            }
                        }
                    }//ListItemComponent
                ]
            }
        }//Container
    }//Container
}//Page
