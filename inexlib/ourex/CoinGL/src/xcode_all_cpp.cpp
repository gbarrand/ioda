#define HAVE_CONFIG_H
#define COIN_INTERNAL
#define COIN_DEBUG 0 

#define COIN_MAXIMUM_TEXTURE2_SIZE gl_COIN_MAXIMUM_TEXTURE2_SIZE
#define COIN_MAXIMUM_TEXTURE3_SIZE gl_COIN_MAXIMUM_TEXTURE3_SIZE
#include "./src/glue/gl.cpp"
#undef COIN_MAXIMUM_TEXTURE2_SIZE
#undef COIN_MAXIMUM_TEXTURE3_SIZE
#include "./src/glue/gl_cgl.cpp"
#include "./src/actions/SoToVRML2Action.cpp"
#include "./src/actions/SoToVRMLAction.cpp"
#include "./src/actions/SoAction.cpp"
#include "./src/actions/SoGetMatrixAction.cpp"
#include "./src/actions/SoRayPickAction.cpp"
#include "./src/actions/SoCallbackAction.cpp"
#include "./src/actions/SoPickAction.cpp"
#include "./src/actions/SoHandleEventAction.cpp"
#include "./src/actions/SoSimplifyAction.cpp"
#include "./src/actions/SoAudioRenderAction.cpp"
#include "./src/actions/SoWriteAction.cpp"
#include "./src/actions/SoGLRenderAction.cpp"
#include "./src/actions/SoSearchAction.cpp"
#include "./src/actions/SoReorganizeAction.cpp"
#include "./src/actions/SoGetPrimitiveCountAction.cpp"
#include "./src/actions/SoBoxHighlightRenderAction.cpp"
#include "./src/actions/SoLineHighlightRenderAction.cpp"
#include "./src/actions/SoGetBoundingBoxAction.cpp"
#include "./src/base/SbBox2d.cpp"
#include "./src/base/SbBox2s.cpp"
#include "./src/base/SbDPMatrix.cpp"
#include "./src/base/SbDict.cpp"
#include "./src/base/SbBox2f.cpp"
#include "./src/base/SbVec4b.cpp"
#include "./src/base/SbHeap.cpp"
#include "./src/base/SbTesselator.cpp"
#include "./src/base/SbSphere.cpp"
#include "./src/base/SbVec4f.cpp"
#include "./src/base/SbBox3f.cpp"
#include "./src/base/SbVec3i32.cpp"
#include "./src/base/SbBox3d.cpp"
#include "./src/base/SbBox3s.cpp"
#include "./src/base/SbViewportRegion.cpp"
#include "./src/base/SbVec4s.cpp"
#include "./src/base/SbVec4d.cpp"
#include "./src/base/SbMatrix.cpp"
#include "./src/base/SbVec3f.cpp"
#include "./src/base/SbVec2b.cpp"
#include "./src/base/SbViewVolume.cpp"
#include "./src/base/SbPlane.cpp"
#include "./src/base/SbVec3s.cpp"
#include "./src/base/SbVec3d.cpp"
#include "./src/base/SbRotation.cpp"
#include "./src/base/SbVec2ui32.cpp"
#include "./src/base/SbBox2i32.cpp"
#include "./src/base/SbVec3ui32.cpp"
#include "./src/base/SbVec2s.cpp"
#include "./src/base/SbVec2d.cpp"
#include "./src/base/SbName.cpp"
#include "./src/base/SbVec2f.cpp"
#include "./src/base/SbVec3b.cpp"
#include "./src/base/SbLine.cpp"
#include "./src/base/SbDPViewVolume.cpp"
#include "./src/base/dynarray.cpp"
#include "./src/base/SbXfBox3d.cpp"
#include "./src/base/SbImage.cpp"
#include "./src/base/SbString.cpp"
#include "./src/base/SbTime.cpp"
#include "./src/base/SbDPLine.cpp"
#include "./src/base/SbVec2us.cpp"
#include "./src/base/SbVec4ub.cpp"
#include "./src/base/SbXfBox3f.cpp"
#include "./src/base/SbDPRotation.cpp"
#include "./src/base/SbVec2ub.cpp"
#include "./src/base/SbVec2i32.cpp"
#include "./src/base/SbCylinder.cpp"
#include "./src/base/SbVec4us.cpp"
#include "./src/base/SbGLUTessellator.cpp"
#include "./src/base/SbVec3us.cpp"
#include "./src/base/SbClip.cpp"
#include "./src/base/SbDPPlane.cpp"
#include "./src/base/SbColor4f.cpp"
#include "./src/base/SbOctTree.cpp"
#include "./src/base/SbColor.cpp"
#include "./src/base/SbVec3ub.cpp"
#include "./src/base/SbVec4i32.cpp"
#include "./src/base/SbVec4ui32.cpp"
#include "./src/base/SbBSPTree.cpp"
#include "./src/base/SbBox3i32.cpp"
#include "./src/upgraders/SoShapeHintsV10.cpp"
#include "./src/upgraders/SoPackedColorV20.cpp"
#include "./src/upgraders/SoUpgrader.cpp"
#include "./src/bundles/SoBundle.cpp"
#include "./src/bundles/SoTextureCoordinateBundle.cpp"
#include "./src/bundles/SoMaterialBundle.cpp"
#include "./src/bundles/SoNormalBundle.cpp"
#include "./src/caches/SoVBOCache.cpp"
#include "./src/caches/SoCache.cpp"
#include "./src/caches/SoGLRenderCache.cpp"
#include "./src/caches/SoConvexDataCache.cpp"
#include "./src/caches/SoShaderProgramCache.cpp"
#include "./src/caches/normalcache_numcoords_hack.cpp"
#include "./src/caches/SoGLCacheList.cpp"
#include "./src/caches/SoBoundingBoxCache.cpp"
#include "./src/caches/SoTextureCoordinateCache.cpp"
#include "./src/caches/SoNormalCache.cpp"
#include "./src/caches/SoGlyphCache.cpp"
#include "./src/caches/SoPrimitiveVertexCache.cpp"
#include "./src/details/SoTextDetail.cpp"
#include "./src/details/SoCylinderDetail.cpp"
#include "./src/details/SoConeDetail.cpp"
#include "./src/details/SoPointDetail.cpp"
#include "./src/details/SoFaceDetail.cpp"
#include "./src/details/SoNodeKitDetail.cpp"
#include "./src/details/SoLineDetail.cpp"
#include "./src/details/SoDetail.cpp"
#include "./src/details/SoCubeDetail.cpp"
#include "./src/draggers/SoRotateDiscDragger.cpp"
#include "./src/draggers/SoTabBoxDragger.cpp"
#include "./src/draggers/SoDragger.cpp"
#include "./src/draggers/SoTransformBoxDragger.cpp"
#include "./src/draggers/SoScale1Dragger.cpp"
#include "./src/draggers/SoTabPlaneDragger.cpp"
#include "./src/draggers/SoRotateCylindricalDragger.cpp"
#include "./src/draggers/SoScale2UniformDragger.cpp"
#include "./src/draggers/SoTranslate2Dragger.cpp"
#include "./src/draggers/SoSpotLightDragger.cpp"
#include "./src/draggers/SoScaleUniformDragger.cpp"
#include "./src/draggers/SoTranslate1Dragger.cpp"
#include "./src/draggers/SoTransformerDragger.cpp"
#include "./src/draggers/SoPointLightDragger.cpp"
#include "./src/draggers/SoJackDragger.cpp"
#include "./src/draggers/SoHandleBoxDragger.cpp"
#include "./src/draggers/SoDragPointDragger.cpp"
#include "./src/draggers/SoScale2Dragger.cpp"
#include "./src/draggers/SoCenterballDragger.cpp"
#include "./src/draggers/SoDirectionalLightDragger.cpp"
#include "./src/draggers/SoRotateSphericalDragger.cpp"
#include "./src/draggers/SoTrackballDragger.cpp"
#include "./src/elements/SoMultiTextureEnabledElement.cpp"
#include "./src/elements/SoDecimationTypeElement.cpp"
#include "./src/elements/SoBumpMapCoordinateElement.cpp"
#include "./src/elements/SoDiffuseColorElement.cpp"
#include "./src/elements/SoMultiTextureImageElement.cpp"
#include "./src/elements/SoTransparencyElement.cpp"
#include "./src/elements/SoLineWidthElement.cpp"
#include "./src/elements/SoOverrideElement.cpp"
#include "./src/elements/SoClipPlaneElement.cpp"
#include "./src/elements/SoModelMatrixElement.cpp"
#include "./src/elements/SoLazyElement.cpp"
#include "./src/elements/SoPickRayElement.cpp"
#include "./src/elements/SoCullElement.cpp"
#include "./src/elements/SoListenerGainElement.cpp"
#include "./src/elements/SoAccumulatedElement.cpp"
#include "./src/elements/SoViewportRegionElement.cpp"
#include "./src/elements/SoMaterialBindingElement.cpp"
#include "./src/elements/SoTextureCombineElement.cpp"
#include "./src/elements/SoAnnoText3RenderPrintElement.cpp"
#include "./src/elements/SoComplexityTypeElement.cpp"
#include "./src/elements/SoBumpMapElement.cpp"
#include "./src/elements/SoFontNameElement.cpp"
#include "./src/elements/SoPointSizeElement.cpp"
#include "./src/elements/SoBBoxModelMatrixElement.cpp"
#include "./src/elements/SoTextureEnabledElement.cpp"
#include "./src/elements/SoTextureScalePolicyElement.cpp"
#include "./src/elements/SoListenerOrientationElement.cpp"
#include "./src/elements/SoListenerDopplerElement.cpp"
#include "./src/elements/SoCoordinateElement.cpp"
#include "./src/elements/SoTextureCoordinateElement.cpp"
#include "./src/elements/SoFloatElement.cpp"
#include "./src/elements/SoShapeStyleElement.cpp"
#include "./src/elements/SoPickStyleElement.cpp"
#include "./src/elements/SoViewVolumeElement.cpp"
#include "./src/elements/SoShapeHintsElement.cpp"
#include "./src/elements/SoUnitsElement.cpp"
#include "./src/elements/SoLightModelElement.cpp"
#include "./src/elements/SoShininessElement.cpp"
#include "./src/elements/SoLightElement.cpp"
#include "./src/elements/SoFocalDistanceElement.cpp"
#include "./src/elements/SoTextureOverrideElement.cpp"
#include "./src/elements/SoListenerPositionElement.cpp"
#include "./src/elements/SoComplexityElement.cpp"
#include "./src/elements/SoNormalElement.cpp"
#include "./src/elements/SoCacheElement.cpp"
#include "./src/elements/SoProfileElement.cpp"
#include "./src/elements/SoLinePatternElement.cpp"
#include "./src/elements/SoNormalBindingElement.cpp"
#include "./src/elements/SoAnnoText3FontSizeHintElement.cpp"
#include "./src/elements/SoMultiTextureCoordinateElement.cpp"
#include "./src/elements/SoTextureScaleQualityElement.cpp"
#include "./src/elements/SoTextOutlineEnabledElement.cpp"
#include "./src/elements/SoElement.cpp"
#include "./src/elements/SoPolygonOffsetElement.cpp"
#include "./src/elements/SoLocalBBoxMatrixElement.cpp"
#include "./src/elements/SoProjectionMatrixElement.cpp"
#include "./src/elements/SoFontSizeElement.cpp"
#include "./src/elements/GL/SoGLVBOElement.cpp"
#include "./src/elements/GL/SoGLTextureEnabledElement.cpp"
#include "./src/elements/GL/SoGLDrawStyleElement.cpp"
#include "./src/elements/GL/SoGLViewingMatrixElement.cpp"
#include "./src/elements/GL/SoGLViewportRegionElement.cpp"
#include "./src/elements/GL/SoGLRenderPassElement.cpp"
#include "./src/elements/GL/SoGLMultiTextureImageElement.cpp"
#include "./src/elements/GL/SoGLEnvironmentElement.cpp"
#include "./src/elements/GL/SoGLPolygonOffsetElement.cpp"
#include "./src/elements/GL/SoGLMultiTextureEnabledElement.cpp"
#include "./src/elements/GL/SoGLTexture3EnabledElement.cpp"
#include "./src/elements/GL/SoGLDisplayList.cpp"
#include "./src/elements/GL/SoGLColorIndexElement.cpp"
#include "./src/elements/GL/SoGLTextureMatrixElement.cpp"
#include "./src/elements/GL/SoGLTextureCoordinateElement.cpp"
#include "./src/elements/GL/SoGLMultiTextureMatrixElement.cpp"
#include "./src/elements/GL/SoGLPointSizeElement.cpp"
#include "./src/elements/GL/SoGLNormalElement.cpp"
#include "./src/elements/GL/SoGLProjectionMatrixElement.cpp"
#include "./src/elements/GL/SoGLMultiTextureCoordinateElement.cpp"
#include "./src/elements/GL/SoGLModelMatrixElement.cpp"
#include "./src/elements/GL/SoGLLineWidthElement.cpp"
#include "./src/elements/GL/SoGLShapeHintsElement.cpp"
#include "./src/elements/GL/SoGLClipPlaneElement.cpp"
#include "./src/elements/GL/SoGLCoordinateElement.cpp"
#include "./src/elements/GL/SoGLLinePatternElement.cpp"
#include "./src/elements/GL/SoGLCacheContextElement.cpp"
#include "./src/elements/GL/SoGLTextureImageElement.cpp"
#include "./src/elements/GL/SoGLUpdateAreaElement.cpp"
#include "./src/elements/GL/SoGLLightIdElement.cpp"
#include "./src/elements/GL/SoGLLazyElement.cpp"
#include "./src/elements/SoTextureUnitElement.cpp"
#include "./src/elements/SoTextureMatrixElement.cpp"
#include "./src/elements/SoDrawStyleElement.cpp"
#include "./src/elements/SoReplacedElement.cpp"
#include "./src/elements/SoViewingMatrixElement.cpp"
#include "./src/elements/SoWindowElement.cpp"
#include "./src/elements/SoTextureQualityElement.cpp"
#include "./src/elements/SoProfileCoordinateElement.cpp"
#include "./src/elements/SoTextureCoordinateBindingElement.cpp"
#include "./src/elements/SoBumpMapMatrixElement.cpp"
#include "./src/elements/SoAmbientColorElement.cpp"
#include "./src/elements/SoSpecularColorElement.cpp"
#include "./src/elements/SoDecimationPercentageElement.cpp"
#include "./src/elements/SoAnnoText3CharOrientElement.cpp"
#include "./src/elements/SoTextureImageElement.cpp"
#include "./src/elements/SoSoundElement.cpp"
#include "./src/elements/SoSwitchElement.cpp"
#include "./src/elements/SoLightAttenuationElement.cpp"
#include "./src/elements/SoTexture3EnabledElement.cpp"
#include "./src/elements/SoInt32Element.cpp"
#include "./src/elements/SoCreaseAngleElement.cpp"
#include "./src/elements/SoEmissiveColorElement.cpp"
#include "./src/elements/SoMultiTextureMatrixElement.cpp"
#include "./src/elements/SoEnvironmentElement.cpp"
#include "./src/engines/SoInterpolateVec3f.cpp"
#include "./src/engines/SoComposeRotation.cpp"
#include "./src/engines/SoTransformVec3f.cpp"
#include "./src/engines/SoEngine.cpp"
#include "./src/engines/SoOnOff.cpp"
#include "./src/engines/SoSelectOne.cpp"
#include "./src/engines/SoInterpolateVec2f.cpp"
#include "./src/engines/SoGate.cpp"
#include "./src/engines/SoTriggerAny.cpp"
#include "./src/engines/SoFieldConverter.cpp"
#include "./src/engines/SoDecomposeMatrix.cpp"
#include "./src/engines/SoComposeMatrix.cpp"
#include "./src/engines/SoInterpolateRotation.cpp"
#include "./src/engines/SoInterpolateVec4f.cpp"
#include "./src/engines/SoComposeRotationFromTo.cpp"
#include "./src/engines/SoInterpolateFloat.cpp"
#include "./src/engines/SoTimeCounter.cpp"
#include "./src/engines/SoOneShot.cpp"
#include "./src/engines/SoDecomposeRotation.cpp"
#include "./src/engines/SoConvertAll.cpp"
#include "./src/engines/SoEngineOutput.cpp"
#include "./src/engines/SoComputeBoundingBox.cpp"
#include "./src/engines/SoDecomposeVec3f.cpp"
#include "./src/engines/SoNodeEngine.cpp"
#include "./src/engines/SoComposeVec4f.cpp"
#include "./src/engines/SoDecomposeVec2f.cpp"
#include "./src/engines/SoBoolOperation.cpp"
#include "./src/engines/SoOutputData.cpp"
#include "./src/engines/SoElapsedTime.cpp"
#include "./src/engines/SoCounter.cpp"
#include "./src/engines/SoComposeVec3f.cpp"
#include "./src/engines/SoConcatenate.cpp"
#include "./src/engines/SoInterpolate.cpp"
#include "./src/engines/SoCalculator.cpp"
#include "./src/engines/SoComposeVec2f.cpp"
#include "./src/engines/SoDecomposeVec4f.cpp"
#include "./src/errors/SoMemoryError.cpp"
#include "./src/errors/SoReadError.cpp"
#include "./src/errors/SoError.cpp"
#include "./src/errors/SoDebugError.cpp"
#include "./src/events/SoEvent.cpp"
#include "./src/events/SoSpaceballButtonEvent.cpp"
#include "./src/events/SoKeyboardEvent.cpp"
#include "./src/events/SoButtonEvent.cpp"
#include "./src/events/SoLocation2Event.cpp"
#include "./src/events/SoMouseButtonEvent.cpp"
#include "./src/events/SoMotion3Event.cpp"
#include "./src/fields/SoSFVec4ub.cpp"
#include "./src/fields/SoSFBitMask.cpp"
#include "./src/fields/SoMFVec3i32.cpp"
#include "./src/fields/SoMFVec4ub.cpp"
#include "./src/fields/SoMFString.cpp"
#include "./src/fields/SoSFNode.cpp"
#include "./src/fields/SoSFString.cpp"
#include "./src/fields/SoSFImage.cpp"
#include "./src/fields/SoMFNode.cpp"
#include "./src/fields/SoMFColorRGBA.cpp"
#include "./src/fields/SoSFVec4us.cpp"
#include "./src/fields/SoMFVec4us.cpp"
#include "./src/fields/SoSFVec2i32.cpp"
#include "./src/fields/SoFieldData.cpp"
#include "./src/fields/SoMFRotation.cpp"
#include "./src/fields/SoSFName.cpp"
#include "./src/fields/SoSFColor.cpp"
#include "./src/fields/SoSFImage3.cpp"
#include "./src/fields/SoMFUInt32.cpp"
#include "./src/fields/SoSFUInt32.cpp"
#include "./src/fields/SoSFTrigger.cpp"
#include "./src/fields/SoSFBox3i32.cpp"
#include "./src/fields/SoField.cpp"
#include "./src/fields/SoMFUShort.cpp"
#include "./src/fields/SoMFName.cpp"
#include "./src/fields/SoSFVec4i32.cpp"
#include "./src/fields/SoSFRotation.cpp"
#include "./src/fields/SoMFColor.cpp"
#include "./src/fields/SoSFUShort.cpp"
#include "./src/fields/SoMFVec4f.cpp"
#include "./src/fields/SoSField.cpp"
#include "./src/fields/SoSFBox2f.cpp"
#include "./src/fields/SoMFTime.cpp"
#include "./src/fields/SoSFVec4b.cpp"
#include "./src/fields/SoMFInt32.cpp"
#include "./src/fields/SoSFDouble.cpp"
#include "./src/fields/SoSFBox2d.cpp"
#include "./src/fields/SoSFBox2s.cpp"
#include "./src/fields/SoSFShort.cpp"
#include "./src/fields/SoMFVec2i32.cpp"
#include "./src/fields/SoMFBool.cpp"
#include "./src/fields/SoMFDouble.cpp"
#include "./src/fields/SoMFVec4d.cpp"
#include "./src/fields/SoMFFloat.cpp"
#include "./src/fields/SoMFVec4s.cpp"
#include "./src/fields/SoSFBool.cpp"
#include "./src/fields/SoSFBox3d.cpp"
#include "./src/fields/SoSFBox3s.cpp"
#include "./src/fields/SoSFColorRGBA.cpp"
#include "./src/fields/SoMFShort.cpp"
#include "./src/fields/SoSFVec4s.cpp"
#include "./src/fields/SoSFVec4d.cpp"
#include "./src/fields/SoSFFloat.cpp"
#include "./src/fields/SoSFVec4f.cpp"
#include "./src/fields/SoSFEngine.cpp"
#include "./src/fields/SoSFTime.cpp"
#include "./src/fields/SoSFVec3i32.cpp"
#include "./src/fields/SoMFBitMask.cpp"
#include "./src/fields/SoSFBox3f.cpp"
#include "./src/fields/SoMFEngine.cpp"
#include "./src/fields/SoSFInt32.cpp"
#include "./src/fields/SoMField.cpp"
#include "./src/fields/SoMFVec4b.cpp"
#include "./src/fields/SoMFPath.cpp"
#include "./src/fields/SoSFVec3s.cpp"
#include "./src/fields/SoSFVec3d.cpp"
#include "./src/fields/SoMFVec4i32.cpp"
#include "./src/fields/SoFieldContainer.cpp"
#include "./src/fields/SoMFVec2d.cpp"
#include "./src/fields/SoMFVec2s.cpp"
#include "./src/fields/SoMFVec3b.cpp"
#include "./src/fields/SoMFVec2f.cpp"
#include "./src/fields/SoSFEnum.cpp"
#include "./src/fields/SoGlobalField.cpp"
#include "./src/fields/SoSFVec4ui32.cpp"
#include "./src/fields/SoMFMatrix.cpp"
#include "./src/fields/SoSFMatrix.cpp"
#include "./src/fields/SoSFPlane.cpp"
#include "./src/fields/SoSFVec2b.cpp"
#include "./src/fields/SoSFVec3f.cpp"
#include "./src/fields/shared.cpp"
#include "./src/fields/SoSFVec3b.cpp"
#include "./src/fields/SoSFVec2f.cpp"
#include "./src/fields/SoMFVec4ui32.cpp"
#include "./src/fields/SoMFEnum.cpp"
#include "./src/fields/SoMFPlane.cpp"
#include "./src/fields/SoMFVec2b.cpp"
#include "./src/fields/SoMFVec3f.cpp"
#include "./src/fields/SoMFVec3d.cpp"
#include "./src/fields/SoMFVec3s.cpp"
#include "./src/fields/SoSFPath.cpp"
#include "./src/fields/SoSFBox2i32.cpp"
#include "./src/fields/SoSFVec2s.cpp"
#include "./src/fields/SoSFVec2d.cpp"
#include "./src/manips/SoDirectionalLightManip.cpp"
#include "./src/manips/commoncode.cpp"
#include "./src/manips/SoTransformBoxManip.cpp"
#include "./src/manips/SoHandleBoxManip.cpp"
#include "./src/manips/SoTabBoxManip.cpp"
#include "./src/manips/SoCenterballManip.cpp"
#include "./src/manips/SoTransformManip.cpp"
#include "./src/manips/SoTransformerManip.cpp"
#include "./src/manips/SoTrackballManip.cpp"
#include "./src/manips/SoSpotLightManip.cpp"
#include "./src/manips/SoJackManip.cpp"
#include "./src/manips/SoPointLightManip.cpp"
#include "./src/manips/SoClipPlaneManip.cpp"
#include "./src/lists/SoPathList.cpp"
#include "./src/lists/SoBaseList.cpp"
#include "./src/lists/SoCallbackList.cpp"
#include "./src/lists/SbVec3fList.cpp"
#include "./src/lists/SoPickedPointList.cpp"
#include "./src/lists/SbPList.cpp"
#include "./src/lists/SoEnabledElementsList.cpp"
#include "./src/lists/SoTypeList.cpp"
#include "./src/lists/SoDetailList.cpp"
#include "./src/lists/SoActionMethodList.cpp"
#include "./src/lists/SoAuditorList.cpp"
#include "./src/lists/SoNodeList.cpp"
#include "./src/lists/SoEngineList.cpp"
#include "./src/nodekits/SoLightKit.cpp"
#include "./src/nodekits/SoBaseKit.cpp"
#include "./src/nodekits/SoSceneKit.cpp"
#include "./src/nodekits/SoCameraKit.cpp"
#include "./src/nodekits/SoShapeKit.cpp"
#include "./src/nodekits/SoNodeKitPath.cpp"
#include "./src/nodekits/SoAppearanceKit.cpp"
#include "./src/nodekits/SoNodeKit.cpp"
#include "./src/nodekits/SoInteractionKit.cpp"
#include "./src/nodekits/SoNodeKitListPart.cpp"
#include "./src/nodekits/SoSeparatorKit.cpp"
#include "./src/nodekits/SoNodekitCatalog.cpp"
#include "./src/nodekits/SoWrapperKit.cpp"
#include "./src/nodes/SoVertexProperty.cpp"
#include "./src/nodes/SoTextureCoordinateSphere.cpp"
#include "./src/nodes/SoLocateHighlight.cpp"
#include "./src/nodes/SoPackedColor.cpp"
#include "./src/nodes/SoSceneTexture2.cpp"
#include "./src/nodes/SoMatrixTransform.cpp"
#include "./src/nodes/SoBumpMapTransform.cpp"
#include "./src/nodes/SoScale.cpp"
#include "./src/nodes/SoTexture2.cpp"
#include "./src/nodes/SoBlinker.cpp"
#include "./src/nodes/SoCallback.cpp"
#include "./src/nodes/SoTexture3.cpp"
#include "./src/nodes/SoTexture2Transform.cpp"
#include "./src/nodes/SoTextureScalePolicy.cpp"
#include "./src/nodes/SoGroup.cpp"
#include "./src/nodes/SoPathSwitch.cpp"
#include "./src/nodes/SoProfile.cpp"
#include "./src/nodes/SoLightModel.cpp"
#define get_current_time SoRotor_get_current_time 
#include "./src/nodes/SoRotor.cpp"
#undef get_current_time
#include "./src/nodes/SoSeparator.cpp"
#include "./src/nodes/SoPickStyle.cpp"
#include "./src/nodes/SoTexture3Transform.cpp"
#include "./src/nodes/SoWWWAnchor.cpp"
#include "./src/nodes/SoSpotLight.cpp"
#include "./src/nodes/SoSurroundScale.cpp"
#include "./src/nodes/SoFile.cpp"
#include "./src/nodes/SoTextureMatrixTransform.cpp"
#include "./src/nodes/SoLabel.cpp"
#include "./src/nodes/SoTextureCoordinate3.cpp"
#include "./src/nodes/SoTextureCoordinate2.cpp"
#include "./src/nodes/SoPolygonOffset.cpp"
#include "./src/nodes/SoArray.cpp"
#include "./src/nodes/SoTextureCoordinateCylinder.cpp"
#include "./src/nodes/SoWWWInline.cpp"
#include "./src/nodes/SoEnvironment.cpp"
#include "./src/nodes/SoClipPlane.cpp"
#include "./src/nodes/SoResetTransform.cpp"
#include "./src/nodes/SoTextureUnit.cpp"
#include "./src/nodes/SoTransparencyType.cpp"
#include "./src/nodes/SoLevelOfDetail.cpp"
#include "./src/nodes/SoInfo.cpp"
#include "./src/nodes/SoComplexity.cpp"
#include "./src/nodes/SoTransformSeparator.cpp"
#include "./src/nodes/SoBumpMap.cpp"
#include "./src/nodes/SoDirectionalLight.cpp"
#include "./src/nodes/SoColorIndex.cpp"
#include "./src/nodes/SoSwitch.cpp"
#include "./src/nodes/SoPointLight.cpp"
#include "./src/nodes/SoProfileCoordinate3.cpp"
#include "./src/nodes/SoEventCallback.cpp"
#include "./src/nodes/SoProfileCoordinate2.cpp"
#include "./src/nodes/SoLOD.cpp"
#include "./src/nodes/SoLight.cpp"
#include "./src/nodes/SoRotationXYZ.cpp"
#include "./src/nodes/SoMultipleCopy.cpp"
#include "./src/nodes/SoTextureCoordinateCube.cpp"
#include "./src/nodes/SoNormalBinding.cpp"
#include "./src/nodes/SoListener.cpp"
#include "./src/nodes/SoTransform.cpp"
#include "./src/nodes/SoShapeHints.cpp"
#include "./src/nodes/SoNormal.cpp"
#include "./src/nodes/SoUnknownNode.cpp"
#include "./src/nodes/SoNurbsProfile.cpp"
#include "./src/nodes/SoTranslation.cpp"
#include "./src/nodes/SoTextureCoordinatePlane.cpp"
#include "./src/nodes/SoBumpMapCoordinate.cpp"
#include "./src/nodes/SoTextureCombine.cpp"
#include "./src/nodes/SoTextureCoordinateBinding.cpp"
#include "./src/nodes/SoShuttle.cpp"
#include "./src/nodes/SoTextureCoordinateFunction.cpp"
#include "./src/nodes/SoFrustumCamera.cpp"
#include "./src/nodes/SoCamera.cpp"
#include "./src/nodes/SoRotation.cpp"
#include "./src/nodes/SoTextureCoordinateDefault.cpp"
#include "./src/nodes/SoUnits.cpp"
#include "./src/nodes/SoOrthographicCamera.cpp"
#include "./src/nodes/SoCoordinate4.cpp"
#include "./src/nodes/SoDrawStyle.cpp"
#include "./src/nodes/SoTransformation.cpp"
#include "./src/nodes/SoTextureCoordinateEnvironment.cpp"
#include "./src/nodes/SoAntiSquish.cpp"
#include "./src/nodes/SoFontStyle.cpp"
#include "./src/nodes/SoBaseColor.cpp"
#include "./src/nodes/SoPendulum.cpp"
#include "./src/nodes/SoMaterialBinding.cpp"
#include "./src/nodes/SoLinearProfile.cpp"
#include "./src/nodes/SoTextureCubeMap.cpp"
#include "./src/nodes/SoMaterial.cpp"
#include "./src/nodes/SoAnnotation.cpp"
#include "./src/nodes/SoExtSelection.cpp"
#include "./src/nodes/SoPerspectiveCamera.cpp"
#include "./src/nodes/SoFont.cpp"
#include "./src/nodes/SoCoordinate3.cpp"
#include "./src/nodes/SoNode.cpp"
#include "./src/nodes/SoSelection.cpp"
#include "./src/projectors/SbCylinderSectionProjector.cpp"
#include "./src/projectors/SbCylinderPlaneProjector.cpp"
#include "./src/projectors/SbCylinderSheetProjector.cpp"
#include "./src/projectors/SbSphereProjector.cpp"
#include "./src/projectors/SbProjector.cpp"
#include "./src/projectors/SbCylinderProjector.cpp"
#include "./src/projectors/SbSpherePlaneProjector.cpp"
#include "./src/projectors/SbSphereSectionProjector.cpp"
#include "./src/projectors/SbSphereSheetProjector.cpp"
#include "./src/projectors/SbLineProjector.cpp"
#include "./src/projectors/SbPlaneProjector.cpp"
#include "./src/sensors/SoSensorManager.cpp"
#include "./src/sensors/SoDataSensor.cpp"
#include "./src/sensors/SoIdleSensor.cpp"
#include "./src/sensors/SoSensor.cpp"
#include "./src/sensors/SoFieldSensor.cpp"
#include "./src/sensors/SoDelayQueueSensor.cpp"
#include "./src/sensors/SoTimerSensor.cpp"
#include "./src/sensors/SoTimerQueueSensor.cpp"
#include "./src/sensors/SoAlarmSensor.cpp"
#include "./src/sensors/SoOneShotSensor.cpp"
#include "./src/sensors/SoNodeSensor.cpp"
#include "./src/sensors/SoPathSensor.cpp"
#include "./src/misc/SoPick.cpp"
#include "./src/misc/SoOffscreenWGLData.cpp"
#include "./src/misc/SoPath.cpp"
#include "./src/misc/SoOffscreenRenderer.cpp"
#include "./src/misc/CoinOffscreenGLCanvas.cpp"
#include "./src/misc/SoChildList.cpp"
#include "./src/misc/SoVBO.cpp"
#include "./src/misc/SoProtoInstance.cpp"
#include "./src/misc/CoinStaticObjectInDLL.cpp"
#include "./src/misc/SoLightPath.cpp"
#include "./src/misc/SoInteraction.cpp"
#include "./src/misc/SoPickedPoint.cpp"
#include "./src/misc/SoNotification.cpp"
#define compute_log SoGLImage_compute_log
#include "./src/misc/SoGLImage.cpp"
#undef compute_log
#include "./src/misc/SoShaderGenerator.cpp"
#include "./src/misc/SoContextHandler.cpp"
#include "./src/misc/SoType.cpp"
#include "./src/misc/SoOffscreenCGData.cpp"
#include "./src/misc/SoPrimitiveVertex.cpp"
#include "./src/misc/SoDebug.cpp"
#include "./src/misc/SoNotRec.cpp"
#include "./src/misc/SoDB.cpp"
#include "./src/misc/SoState.cpp"
#include "./src/misc/SoVertexArrayIndexer.cpp"
#include "./src/misc/SoNormalGenerator.cpp"
#include "./src/misc/SoGL.cpp"
#include "./src/misc/SoGlyph.cpp"
#include "./src/misc/SoTempPath.cpp"
#include "./src/misc/SoProto.cpp"
#include "./src/misc/SoGLCubeMapImage.cpp"
#include "./src/misc/SoOffscreenGLXData.cpp"
#include "./src/misc/SoGenerate.cpp"
#include "./src/misc/SoOffscreenAGLData.cpp"
#include "./src/misc/PointerMap.cpp"
#include "./src/misc/SoSceneManager.cpp"
#include "./src/misc/SoAudioDevice.cpp"
#include "./src/misc/AudioTools.cpp"
#include "./src/misc/SoBase.cpp"
#include "./src/misc/SoFullPath.cpp"
#include "./src/misc/SoLockManager.cpp"
#include "./src/misc/SoGLBigImage.cpp"
#include "./src/misc/SoCompactPathList.cpp"
#include "./src/hardcopy/VectorizeAction.cpp"
#include "./src/hardcopy/PSVectorOutput.cpp"
#include "./src/hardcopy/VectorOutput.cpp"
#include "./src/hardcopy/VectorizeActionP.cpp"
#include "./src/hardcopy/VectorizePSAction.cpp"
#include "./src/hardcopy/HardCopy.cpp"
#include "./src/shapenodes/SoNurbsSurface.cpp"
#include "./src/shapenodes/SoVertexShape.cpp"
#include "./src/shapenodes/SoCone.cpp"
#include "./src/shapenodes/SoFaceSet.cpp"
#include "./src/shapenodes/SoLineSet.cpp"
#include "./src/shapenodes/SoAsciiText.cpp"
#include "./src/shapenodes/SoCylinder.cpp"
#include "./src/shapenodes/SoSphere.cpp"
#include "./src/shapenodes/SoMarkerSet.cpp"
#include "./src/shapenodes/SoIndexedNurbsCurve.cpp"
#include "./src/shapenodes/SoIndexedLineSet.cpp"
#include "./src/shapenodes/SoTriangleStripSet.cpp"
#include "./src/shapenodes/SoIndexedFaceSet.cpp"
#include "./src/shapenodes/SoPointSet.cpp"
#include "./src/shapenodes/soshape_bigtexture.cpp"
#include "./src/shapenodes/SoCube.cpp"
#include "./src/shapenodes/SoShape.cpp"
#include "./src/shapenodes/soshape_bumprender.cpp"
#include "./src/shapenodes/SoIndexedNurbsSurface.cpp"
#include "./src/shapenodes/SoIndexedShape.cpp"
#include "./src/shapenodes/SoQuadMesh.cpp"
#include "./src/shapenodes/SoIndexedTriangleStripSet.cpp"
#include "./src/shapenodes/soshape_primdata.cpp"
#include "./src/shapenodes/SoNurbsCurve.cpp"
#include "./src/shapenodes/soshape_trianglesort.cpp"
#include "./src/shapenodes/SoText3.cpp"
#include "./src/shapenodes/SoImage.cpp"
#include "./src/shapenodes/SoNonIndexedShape.cpp"
#include "./src/shapenodes/SoText2.cpp"
#include "./src/collision/SoIntersectionDetectionAction.cpp"
#include "./src/collision/SbTri3f.cpp"
#include "./src/io/SoTranReceiver.cpp"
#include "./src/io/SoInput_Reader.cpp"
#include "./src/io/SoTranSender.cpp"
#include "./src/io/SoByteStream.cpp"
#include "./src/io/SoOutput.cpp"
#include "./src/io/SoInput_FileInfo.cpp"
#include "./src/io/SoWriterefCounter.cpp"
#include "./src/io/SoInput.cpp"
#include "./src/io/SoOutput_Writer.cpp"
#include "./src/vrml97/DirectionalLight.cpp"
#include "./src/vrml97/VisibilitySensor.cpp"
#include "./src/vrml97/LOD.cpp"
#include "./src/vrml97/Billboard.cpp"
#include "./src/vrml97/Anchor.cpp"
#include "./src/vrml97/ColorInterpolator.cpp"
#include "./src/vrml97/NormalInterpolator.cpp"
#include "./src/vrml97/CoordinateInterpolator.cpp"
#include "./src/vrml97/Geometry.cpp"
#include "./src/vrml97/Interpolator.cpp"
#include "./src/vrml97/Cylinder.cpp"
#include "./src/vrml97/Extrusion.cpp"
#include "./src/vrml97/Parent.cpp"
#include "./src/vrml97/PixelTexture.cpp"
#include "./src/vrml97/IndexedLineSet.cpp"
#include "./src/vrml97/IndexedFaceSet.cpp"
#include "./src/vrml97/MovieTexture.cpp"
#include "./src/vrml97/Text.cpp"
#include "./src/vrml97/WorldInfo.cpp"
#include "./src/vrml97/PointLight.cpp"
#include "./src/vrml97/SpotLight.cpp"
#include "./src/vrml97/Appearance.cpp"
#include "./src/vrml97/Box.cpp"
#include "./src/vrml97/Viewpoint.cpp"
#include "./src/vrml97/Script.cpp"
#include "./src/vrml97/TimeSensor.cpp"
#include "./src/vrml97/Light.cpp"
#include "./src/vrml97/Texture.cpp"
#include "./src/vrml97/Normal.cpp"
#include "./src/vrml97/PointSet.cpp"
#include "./src/vrml97/PlaneSensor.cpp"
#include "./src/vrml97/VertexLine.cpp"
#include "./src/vrml97/NavigationInfo.cpp"
#include "./src/vrml97/Switch.cpp"
#include "./src/vrml97/Inline.cpp"
#include "./src/vrml97/Coordinate.cpp"
#include "./src/vrml97/TouchSensor.cpp"
#include "./src/vrml97/VertexPoint.cpp"
#include "./src/vrml97/Color.cpp"
#include "./src/vrml97/DragSensor.cpp"
#include "./src/vrml97/VertexShape.cpp"
#include "./src/vrml97/Cone.cpp"
#include "./src/vrml97/Transform.cpp"
#include "./src/vrml97/Background.cpp"
#include "./src/vrml97/PositionInterpolator.cpp"
#include "./src/vrml97/Shape.cpp"
#include "./src/vrml97/ScalarInterpolator.cpp"
#include "./src/vrml97/AudioClip.cpp"
#include "./src/vrml97/Sound.cpp"
#include "./src/vrml97/CylinderSensor.cpp"
#include "./src/vrml97/TextureCoordinate.cpp"
#include "./src/vrml97/Sensor.cpp"
#include "./src/vrml97/JS_VRMLClasses.cpp"
#include "./src/vrml97/Fog.cpp"
#include "./src/vrml97/Sphere.cpp"
#include "./src/vrml97/OrientationInterpolator.cpp"
#include "./src/vrml97/IndexedLine.cpp"
#include "./src/vrml97/ImageTexture.cpp"
#include "./src/vrml97/Init.cpp"
#include "./src/vrml97/ElevationGrid.cpp"
#include "./src/vrml97/ProximitySensor.cpp"
#include "./src/vrml97/SphereSensor.cpp"
#include "./src/vrml97/FontStyle.cpp"
#include "./src/vrml97/Material.cpp"
#include "./src/vrml97/TextureTransform.cpp"
#include "./src/vrml97/Collision.cpp"
#define ray_intersect Group_ray_intersect
#include "./src/vrml97/Group.cpp"
#undef ray_intersect
#include "./src/vrml97/IndexedShape.cpp"
#include "./src/geo/SbGeoEllipsoid.cpp"
#include "./src/geo/SoGeoSeparator.cpp"
#include "./src/geo/SbUTMProjection.cpp"
#include "./src/geo/SoGeoLocation.cpp"
#include "./src/geo/SbGeoProjection.cpp"
#include "./src/geo/SoGeoOrigin.cpp"
#include "./src/geo/SbPolarStereographic.cpp"
#include "./src/geo/SoGeo.cpp"
#include "./src/geo/SoGeoCoordinate.cpp"
#include "./src/geo/SoGeoElement.cpp"
#include "./src/geo/SbGeoAngle.cpp"
#include "./src/shaders/SoGLSLShaderProgram.cpp"
#include "./src/shaders/SoShaderObject.cpp"
#include "./src/shaders/SoGLCgShaderObject.cpp"
#include "./src/shaders/SoGLARBShaderProgram.cpp"
#include "./src/shaders/SoGLARBShaderParameter.cpp"
#include "./src/shaders/SoGLShaderParameter.cpp"
#include "./src/shaders/SoShaderProgram.cpp"
#include "./src/shaders/SoVertexShader.cpp"
#include "./src/shaders/SoFragmentShader.cpp"
#include "./src/shaders/SoShaderParameter.cpp"
#include "./src/shaders/SoGLShaderProgramElement.cpp"
#include "./src/shaders/SoGeometryShader.cpp"
#include "./src/shaders/SoShader.cpp"
#include "./src/shaders/SoGLShaderProgram.cpp"
#include "./src/shaders/SoGLCgShaderProgram.cpp"
#include "./src/shaders/SoGLSLShaderParameter.cpp"
#include "./src/shaders/SoGLARBShaderObject.cpp"
#include "./src/shaders/SoGLCgShaderParameter.cpp"
#include "./src/shaders/SoGLShaderObject.cpp"
#include "./src/shaders/SoGLSLShaderObject.cpp"
#include "./src/shadows/SoShadowStyle.cpp"
#include "./src/shadows/SoShadowGroup.cpp"
#include "./src/shadows/SoShadowCulling.cpp"
#include "./src/shadows/SoShadowStyleElement.cpp"
#include "./src/shadows/SoGLShadowCullingElement.cpp"
