// Cloud drawing: GameCube particle names and HD renderer setup.
#include "d/d_kankyo_rain_drawSibuki.h"
namespace RainDraw {
/* 02575B6C */
void drawVrkumo(u32 drawMtx, u32 color, u32 images) {

    // Full native caller workspace: the rendering setup objects extend through4C8.
    gabi::Local<CloudWork> work;
    getEnvironment();
    getEnvironment();
    u32 packet = (u32)(at<Environment>(getEnvironment())->mpVrkumoPacket.v),
        camera = (u32)(at<GameInfo>(getGameInfo())->camera);
    getGameInfo();
    bool special = cloud_stage(CloudSpecialStage, ea(&work->stageName), ea(&work->currentStage));
    for (u32 i = 0; i < 3; i++)
        work->packedCloudColor.channels[i] =
            (u8)(at<Environment>(getEnvironment())->mVrkumoColor.channels[i]);
    cloud_sdk::convertColor(ea(&work->floatColor), ea(&work->packedCloudColor));
    cloud_sdk::scaleColor(ea(&work->cloudColor), ea(&work->floatColor),
                          (f32)(at<Environment>(getEnvironment())->mCloudColorScale));
    for (u32 i = 0; i < 3; i++)
        work->packedCenterColor.channels[i] =
            (u8)(at<Environment>(getEnvironment())->mVrkumoCenterColor.channels[i]);
    cloud_sdk::convertColor(ea(&work->floatColor), ea(&work->packedCenterColor));
    cloud_sdk::scaleColor(ea(&work->centerColor), ea(&work->floatColor),
                          (f32)(at<Environment>(getEnvironment())->mCloudCenterColorScale));
    work->color.channels[1] = (f32)(*at<be<f32>>(CloudZero));
    work->color.channels[3] = (f32)(*at<be<f32>>(CloudOne));
    work->color.channels[0] = (f32)(*at<be<f32>>(CloudZero));
    work->color.channels[2] = (f32)(*at<be<f32>>(CloudZero));
    cloud_sdk::constructRenderState(ea(&work->renderState));
    cloud_sdk::prepareProjection(ea(&work->sunProjection));
    u32 sun = (u32)(at<Environment>(getEnvironment())->mpSunPacket.v);
    if (sun)
        cloud_sdk::project(ea(&at<SunPacket>(sun)->mPos[0]), ea(&work->projectedSun),
                           ea(&work->sunProjection));
    if (!(u32)(at<GameInfo>(getGameInfo())->view))
        return;
    cloud_sdk::inverse(ea(&at<View>((u32)at<GameInfo>(getGameInfo())->view)->mRotation), ea(&work->view));
    work->bank = gabi::fmuls_ppc((f32)(s16)(at<Camera>(camera)->mBank), (f32)(*at<be<f32>>(BankToRadians)));
    bool farStage = !cloud_stage(CloudFarStage, ea(&work->stageName), ea(&work->farStage));
    if (farStage)
        farStage = cloud_stage_info() != 0;
    f32 farPlane = farStage ? gabi::fsubs_ppc((f32)(at<CloudStageInfo>(cloud_stage_info())->farPlane),
                                              (f32)(*at<be<f32>>(FarPlaneInset)))
                            : (f32)(*at<be<f32>>(CloudFarPlane));
    const u32 manager = CloudRenderer;
    u32 cache = cloud_sdk::shaderCache<u32>(manager),
        material = (u32)(at<CloudResource>((u32)(at<CloudPacket>(packet)->mResource))->material);
    if (material != (u32)(at<ShaderCache>(cache)->material)) {
        u8 flags = (u8)(at<Material>(material)->flags);
        u32 previous = (u32)(at<ShaderCache>(cache)->shader);
        if (flags & 2) {
            at<Material>(material)->flags = flags & ~2;
            cloud_sdk::updateMaterial(material, 0);
        }
        u32 shader = (u32)(at<ShaderResource>((u32)(at<Material>(material)->shaderResource))->shader);
        if (previous != shader)
            cloud_sdk::setShader(shader);
        if ((u32)(at<Material>(material)->size))
            cloud_sdk::setMaterialData((u32)(at<Material>(material)->data),
                                       (u32)(at<Material>(material)->size));
        else
            cloud_sdk::applyMaterial(material);
        at<ShaderCache>(cache)->material = material;
        at<ShaderCache>(cache)->shader = shader;
    }
    for (u32 off = 4; off <= 64; off += 4)
        work->projectionMatrix[off / 4 - 1] = (u32)(at<Renderer>(manager)->projectionWords[off / 4]);
    u32 global = (u32)(*at<be<u32>>(RenderGlobalPointer)), list = (u32)(at<RenderGlobal>(global)->views);
    if ((u32)(at<RenderGlobal>(global)->count) > 1)
        list += 4;
    u32 active = (u32)(*at<be<u32>>(list));
    work->loopScale = (f32)(*at<be<f32>>(CloudLoopScale));
    if (active) {
        u32 view;
        if (!((u32)(at<ActiveView>(active)->flags) & 0x1000)) {
            view = (u32)(at<ActiveView>(active)->view);
            if (!view)
                view = DefaultCloudView;
        } else
            view = (u32)(at<ActiveView>(active)->alternativeView);
        cloud_sdk::constructProjection(ea(&work->projection));
        cloud_sdk::setProjectionBounds(ea(&work->projection), (f32)(at<CloudView>(view)->left),
                                       (f32)(at<CloudView>(view)->right), (f32)(at<CloudView>(view)->top),
                                       (f32)(at<CloudView>(view)->bottom));
        work->projection.viewportWidth = (f32)(at<CloudView>(view)->width);
        work->projection.viewportHeight = (f32)(at<CloudView>(view)->height);
        work->projection.dirty = 1;
        if (!(u32)(*at<be<u32>>(ProjectionInitialized))) {
            *at<be<u32>>(ProjectionInitialized) = 1;
            *at<be<u32>>(ProjectionVtablePointer) = ProjectionVtableAddress;
        }
        if (cloud_has_matrix(view) && view)
            cloud_sdk::setProjectionMatrix(ea(&work->projection), ea(&at<CloudView>(view)->matrix));
        work->projection.nearPlane = (f32)(work->loopScale);
        work->projection.farPlane = (f32)(*at<be<f32>>(CloudProjectionFar));
        work->projection.dirty = 1;
        u32 matrix = cloud_sdk::projectionMatrix<u32>(ea(&work->projection));
        cloud_sdk::copyMatrix(matrix, ea(&work->projectionMatrix));
        cloud_sdk::setProjectionMode(ea(&work->projection), 2);
    }
    u32 setup = ea(&at<CloudPacket>(packet)->mSetup);
    cloud_sdk::drawMatrix(ea(&work->drawMatrix), drawMtx);
    cloud_sdk::setupShader(
        setup, 0, ea(&work->drawMatrix), ea(&work->projectionMatrix),
        ea(&at<RendererProjection>((u32)at<Renderer>(manager)->projectionWords[0])->shaderProjection));
    u32 state = (u32)(at<CloudSetup>(setup)->state);
    for (u32 off = 0; off < 16; off += 4)
        at<CloudShaderState>(state)->color[off / 4] = (u32)(at<be<u32>>(CloudColorWords)[off / 4]);
    at<CloudShaderState>((u32)at<CloudSetup>(setup)->state)->scaleX = (f32)(*at<be<f32>>(CloudUniformScale));
    at<CloudShaderState>((u32)at<CloudSetup>(setup)->state)->scaleY = (f32)(*at<be<f32>>(CloudUniformScale));
    cloud_sdk::selectUniform(setup, 0);
    state = (u32)(at<CloudSetup>(setup)->state);
    u32 uniform = ea(&at<CloudShaderState>(state)->uniforms[(u32)at<CloudShaderState>(state)->selected]),
        resource = (u32)(at<CloudPacket>(packet)->mResource),
        binding = (u32)(at<CloudResource>(resource)->bindingCount)
                      ? (u32)(at<CloudResource>(resource)->bindings)
                      : 0;
    s16 vertex = (s16)(at<ShaderBindings>(binding)->vertex),
        pixel = (s16)(at<ShaderBindings>(binding)->pixel),
        geometry = (s16)(at<ShaderBindings>(binding)->geometry);
    u32 data = (u32)(at<ShaderUniform>(uniform)->data), size = (u32)(at<ShaderUniform>(uniform)->size);
    if (pixel != -1)
        cloud_sdk::setPixelUniform(pixel, size, data);
    if (vertex != -1)
        cloud_sdk::setVertexUniform(vertex, size, data);
    if (geometry != -1)
        cloud_sdk::setGeometryUniform(geometry, size, data);
    cloud_sdk::prepareUniform(cloud_setup_uniform(setup));
    cloud_apply_setup(setup, (u32)at<CloudPacket>(packet)->mResource);

    // HD: only pass 1 is drawn, with three texture layers.
    u32 geometryBase = ea(&at<CloudPacket>(packet)->mGeometry[1]);
    for (s32 layer = 0; layer < 3; layer++) {
        if (layer == 0) {
            work->renderState.cullMode = 23;
            cloud_sdk::applyCullMode(ea(&work->renderState));
        }
        work->renderState.blendMode = 3;
        work->renderState.depthTest = 0;
        work->renderState.depthWrite = 0;
        work->renderState.sourceColorFactor = 4;
        work->renderState.destinationColorFactor = 4;
        work->renderState.sourceAlphaFactor = 5;
        work->renderState.destinationAlphaFactor = 5;
        cloud_sdk::applyBlendMode(ea(&work->renderState));
        work->renderState.clipEnabled = 1;
        cloud_sdk::applyClipMode(ea(&work->renderState));
        work->renderState.enabled = 1;
        work->renderState.secondaryEnabled = 0;
        cloud_sdk::applyEnable(ea(&work->renderState));
        if (layer == 0) {
            cloud_sdk::rotate(ea(&work->rotation), 0x5A,
                              gabi::fmuls_ppc((f32)(work->bank), (f32)(*at<be<f32>>(DegreeToRadians))));
            cloud_sdk::concat(ea(&work->view), ea(&work->rotation), ea(&work->view));
        }
        const f32 one = (f32)(*at<be<f32>>(One)), zero = (f32)(*at<be<f32>>(Zero)),
                  half = (f32)(*at<be<f32>>(Half)), sizeBase = (f32)(*at<be<f32>>(CloudSizeBase)),
                  sizeGain = (f32)(*at<be<f32>>(CloudSizeGain)), layerFactor = (f32)(*at<be<f32>>(AlphaStep)),
                  shapeGain = (f32)(*at<be<f32>>(DirectionBias)), pointMin = (f32)(*at<be<f32>>(Drag)),
                  pitchLimit = (f32)(*at<be<f32>>(CloudPitchLimit));
        work->minimumDrawAlpha = (f32)(*at<be<f32>>(CloudMinimumDrawAlpha));
        work->sizeGain = sizeGain;
        work->minimumAlpha = (f32)(*at<be<f32>>(CloudMinimumAlpha));
        work->maximumX = (f32)(*at<be<f32>>(CloudMaximumX));
        work->minimumY = (f32)(*at<be<f32>>(CloudMinimumY));
        work->maximumY = (f32)(*at<be<f32>>(CloudMaximumY));
        work->minimumX = (f32)(*at<be<f32>>(CloudMinimumX));
        work->projectionScale = (f32)(*at<be<f32>>(CloudProjectionScale));
        work->layerBlendOffset = gabi::fmuls_ppc((f32)layer, layerFactor);
        for (u32 i = 0; i < 100; i++) {
            u32 particle = ea(&at<CloudPacket>(packet)->mInst[i]);
            if (!((f32)(at<VRKUMO_EFF>(particle)->mAlpha) > (f32)(work->minimumAlpha)))
                continue;
            if (!special && (s16)(at<Camera>(camera)->mAngleX) > -6000 &&
                (f32)(at<View>((u32)at<GameInfo>(getGameInfo())->view)->mFovy) >
                    (f32)(*at<be<f32>>(WideFovy))) {
                cloud_sdk::prepareProjection(ea(&work->particleProjection));
                cloud_sdk::subtractVector(ea(&at<Camera>(camera)->mEye), ea(&work->worldPosition),
                                          ea(&at<VRKUMO_EFF>(particle)->mPosition));
                for (u32 off = 0; off < 12; off += 4)
                    work->projectInput.words[off / 4] = (u32)(work->worldPosition.words[off / 4]);
                cloud_sdk::project(ea(&work->projectInput), ea(&work->projectedParticle),
                                   ea(&work->particleProjection));
                if (!((f32)(work->projectedParticle.channels[0]) > (f32)(work->minimumX) &&
                      (f32)(work->projectedParticle.channels[0]) < (f32)(work->maximumX) &&
                      (f32)(work->projectedParticle.channels[1]) > (f32)(work->minimumY) &&
                      (f32)(work->projectedParticle.channels[1]) < (f32)(work->maximumY)))
                    continue;
            }
            f32 variation = gabi::fmuls_ppc((f32)((layer + i) & 15), (f32)(*at<be<f32>>(CloudVariation)));
            f32 shape = -gabi::fmadds(gabi::fmuls_ppc(variation, variation), variation, -one);
            f32 weather = gabi::fmadds(sizeBase, (f32)(at<Environment>(getEnvironment())->mVrkumoStrength),
                                       (f32)(work->sizeGain));
            f32 angularSize = gabi::fmuls_ppc(gabi::fmuls_ppc(shape, weather),
                                              (f32)(at<VRKUMO_EFF>(particle)->mDistFalloff));
            u16 angle = (u16)cloud_sdk::angle<u32>(
                gabi::fmadds((f32)(at<CloudPacket>(packet)->mLoopCount), (f32)(work->loopScale), (f32)layer));
            f32 sine = (f32)(at<SinCos>(SinCosTable + (angle >> 3) * 8)->sine);
            angularSize = gabi::fmadds(gabi::fmuls_ppc(pointMin, angularSize),
                                       gabi::fmuls_ppc(sine, (f32)(at<VRKUMO_EFF>(particle)->mDistFalloff)),
                                       angularSize);
            f32 verticalSize =
                    gabi::fmadds(angularSize, (f32)(at<VRKUMO_EFF>(particle)->mHeight), angularSize),
                smallOffset = gabi::fmuls_ppc(angularSize, (f32)(*at<be<f32>>(CloudSmallOffset))),
                wideOffset = gabi::fmuls_ppc(angularSize, shapeGain);
            f32 pitchOffset = zero, yawOffset = zero;
            if (layer == 1) {
                switch (i & 3) {
                case 0:
                    pitchOffset = smallOffset;
                    yawOffset = wideOffset;
                    break;
                case 1:
                    pitchOffset = -smallOffset;
                    yawOffset = smallOffset;
                    break;
                case 2:
                    pitchOffset = wideOffset;
                    yawOffset = -smallOffset;
                    break;
                case 3:
                    pitchOffset = -wideOffset;
                    yawOffset = wideOffset;
                    break;
                }
            } else if (layer == 2) {
                switch (i & 3) {
                case 0:
                    pitchOffset = wideOffset;
                    yawOffset = smallOffset;
                    break;
                case 1:
                    pitchOffset = -wideOffset;
                    yawOffset = wideOffset;
                    break;
                case 2:
                    pitchOffset = smallOffset;
                    yawOffset = -wideOffset;
                    break;
                case 3:
                    pitchOffset = -smallOffset;
                    yawOffset = smallOffset;
                    break;
                }
            }
            f32 x = (f32)(at<VRKUMO_EFF>(particle)->mPosition.channels[0]),
                y = (f32)(at<VRKUMO_EFF>(particle)->mPosition.channels[1]),
                z = (f32)(at<VRKUMO_EFF>(particle)->mPosition.channels[2]);
            f32 horizontal = cloud_sdk::squareRoot<f32>(gabi::fmadds(x, x, gabi::fmuls_ppc(z, z)));
            f32 yaw = cloud_sdk::atan2<f32>(x, z);
            f32 pitch = cloud_sdk::atan2<f32>(y, horizontal);
            pitch = gabi::fadds_ppc(pitch, pitchOffset);
            yaw = gabi::fadds_ppc(yaw, yawOffset);
            f32 q = pitch / (f32)(*at<be<f32>>(CloudPitchScale));
            if (q > one)
                q = one;
            f32 qcube = gabi::fmuls_ppc(q, gabi::fmuls_ppc(q, q));
            f32 pitchCenter =
                gabi::fmadds(gabi::fmuls_ppc(verticalSize, (f32)(*at<be<f32>>(StarSizeBase))),
                             gabi::fmadds(qcube, (f32)(*at<be<f32>>(CloudPitchGain)), one), pitch);
            f32 sideSize = gabi::fmuls_ppc(gabi::fmuls_ppc(angularSize, sizeBase),
                                           gabi::fmadds(qcube, (f32)(*at<be<f32>>(CloudSideGain)), one));
            f32 tallSize = gabi::fmuls_ppc(gabi::fmuls_ppc(angularSize, sizeBase),
                                           gabi::fadds_ppc(gabi::fadds_ppc(qcube, qcube), one));
            auto corner = [&](u32 output, f32 cp, f32 cy, bool clamp) {
                if (clamp && cp > pitchLimit)
                    cp = pitchLimit;
                f32 c = cloud_sdk::cosine<f32>(cp), ss = cloud_sdk::sine<f32>(cy),
                    vx = gabi::fmuls_ppc(c, ss);
                f32 vy = cloud_sdk::sine<f32>(cp);
                c = cloud_sdk::cosine<f32>(cp);
                f32 cs = cloud_sdk::cosine<f32>(cy), vz = gabi::fmuls_ppc(c, cs);
                at<Vector>(output)->channels[0] = gabi::fmuls_ppc(vx, farPlane);
                at<Vector>(output)->channels[1] = gabi::fmuls_ppc(vy, farPlane);
                at<Vector>(output)->channels[2] = gabi::fmuls_ppc(vz, farPlane);
            };
            corner(ea(&work->vertices), pitchCenter, gabi::fadds_ppc(yaw, sideSize), true);
            corner(ea(&work->vertices.vertices[1]), pitchCenter, gabi::fsubs_ppc(yaw, sideSize), true);
            corner(ea(&work->vertices.vertices[2]), pitch, gabi::fsubs_ppc(yaw, tallSize), false);
            corner(ea(&work->vertices.vertices[3]), pitch, gabi::fadds_ppc(yaw, tallSize), false);
            for (u32 off = 0; off < 48; off += 12)
                cloud_sdk::addVector(ea(&work->vertices) + off, ea(&at<Camera>(camera)->mEye),
                                     ea(&work->vertices) + off);
            f32 fade = one;
            if ((u8)(at<Environment>(getEnvironment())->mVrboxInvisible) == 0 && sun) {
                cloud_sdk::subtractVector(ea(&work->vertices), ea(&work->topDifference),
                                          ea(&work->vertices.vertices[1]));
                cloud_sdk::scaleVector(ea(&work->topDifference), ea(&work->topCenter), half);
                cloud_sdk::subtractVector(ea(&work->vertices.vertices[2]), ea(&work->bottomDifference),
                                          ea(&work->vertices.vertices[3]));
                cloud_sdk::scaleVector(ea(&work->bottomDifference), ea(&work->bottomCenter), half);
                cloud_sdk::subtractVector(ea(&work->topCenter), ea(&work->centerDifference),
                                          ea(&work->bottomCenter));
                cloud_sdk::scaleVector(ea(&work->centerDifference), ea(&work->center), half);
                for (u32 off = 0; off < 12; off += 4)
                    work->centerInput.words[off / 4] = (u32)(work->center.words[off / 4]);
                cloud_sdk::project(ea(&work->centerInput), ea(&work->projectedCenter),
                                   ea(&work->sunProjection));
                work->projectedCenter.channels[2] = (f32)(work->projectedSun.channels[2]);
                cloud_sdk::distanceSquared(ea(&work->projectedSun), ea(&work->projectedCenter));
                fade = cloud_sdk::squareRoot<f32>();
                if (fade < (f32)(*at<be<f32>>(CloudFadeDistance)))
                    fade = zero;
                else if (fade < (f32)(*at<be<f32>>(SkyRange)))
                    fade = gabi::fsubs_ppc(fade, (f32)(*at<be<f32>>(CloudFadeDistance))) /
                           (f32)(*at<be<f32>>(CloudFadeDistance));
                else
                    fade = one;
            }
            f32 blend = gabi::fsubs_ppc(one, (f32)(at<VRKUMO_EFF>(particle)->mDistFalloff));
            if (blend > (f32)(*at<be<f32>>(Drag)))
                blend = gabi::fsubs_ppc(blend, (f32)(work->layerBlendOffset));
            cloud_sdk::subtractColor(ea(&work->colorDifference), ea(&work->cloudColor),
                                     ea(&work->centerColor));
            cloud_sdk::scaleColor(ea(&work->scaledDifference), ea(&work->colorDifference), blend);
            cloud_sdk::addColor(ea(&work->color), ea(&work->centerColor), ea(&work->scaledDifference));
            work->color.channels[3] = gabi::fmuls_ppc((f32)(at<VRKUMO_EFF>(particle)->mAlpha), fade);
            if (!((f32)(at<VRKUMO_EFF>(particle)->mAlpha) > (f32)(work->minimumDrawAlpha)))
                continue;
            u32 mat = ea(&at<CloudPacket>(packet)->mUniforms[i]);
            if (layer == 0) {
                cloud_sdk::setUniformColor(mat, ea(&work->color), 0);
                cloud_sdk::prepareUniform(mat);
                cloud_sdk::scaleColor(ea(&work->highlightColor), ea(&work->color),
                                      (f32)(*at<be<f32>>(DirectionBias)));
                cloud_sdk::setUniformColor(mat, ea(&work->highlightColor), 1);
            }
            cloud_apply_uniform(mat, (u32)at<CloudPacket>(packet)->mResource);
            resource = (u32)(at<CloudPacket>(packet)->mResource);
            u32 textures = (u32)(at<CloudResource>(resource)->textureCount)
                               ? (u32)(at<CloudResource>(resource)->textures)
                               : 0;
            cloud_sdk::bindTexture(ea(&at<CloudPacket>(packet)->mTextures[layer]),
                                   ea(&at<TextureList>(textures)->entries), -1, 0);
            u32 geom = geometryBase, selection = (u32)(at<GeometryBank>(geom)->selected),
                slot = geom + (2 * (layer * 100 + i) + selection) * u32(sizeof(BufferSlot));
            u32 buffer = (u32)(at<BufferSlot>(slot)->buffer);
            if (buffer < buffer + 64) {
                for (u32 cursor = buffer; cursor < buffer + 64; cursor += 32)
                    for (u32 word = 0; word < 32; word += 4)
                        at<CacheLine>(cursor & ~31u)->words[word / 4] = 0;
                slot = geom + (2 * (layer * 100 + i) + (u32)(at<GeometryBank>(geom)->selected)) *
                                  u32(sizeof(BufferSlot));
            }
            buffer = (u32)(at<BufferSlot>(slot)->buffer);
            for (u32 vertexIndex = 0; vertexIndex < 4; vertexIndex++) {
                for (u32 component = 0; component < 3; component++)
                    at<Vertex>(buffer + vertexIndex * u32(sizeof(Vertex)))->position.channels[component] =
                        (f32)(work->vertices.vertices[vertexIndex].channels[component]);
                at<Vertex>(buffer + vertexIndex * u32(sizeof(Vertex)))->u =
                    vertexIndex == 1 || vertexIndex == 2 ? one : zero;
                at<Vertex>(buffer + vertexIndex * u32(sizeof(Vertex)))->v = vertexIndex >= 2 ? one : zero;
            }
            at<GeometryBank>(geom)->selected = (u32)(at<GeometryBank>(geom)->selected) == 0;
            u32 selected = (u32)(at<GeometryBank>(geom)->selected) == 0;
            cloud_sdk::prepareGeometry(
                ea(&at<BufferSlot>(geom + (2 * (layer * 100 + i) + selected) * u32(sizeof(BufferSlot)))
                        ->drawObject));
            at<GeometryBank>(geom)->selected = (u32)(at<GeometryBank>(geom)->selected) == 0;
            u32 draw = CloudDrawAddress;
            if ((u32)(at<CloudDraw>(draw)->indices))
                cloud_sdk::drawIndexed((u32)(at<CloudDraw>(draw)->count), (u32)(at<CloudDraw>(draw)->indices),
                                       (u32)(at<CloudDraw>(draw)->primitive),
                                       (u32)(at<CloudDraw>(draw)->indexType), 0, 1);
        }
    }
    cloud_sdk::restoreFog();
    u32 flushBase = ea(&at<CloudPacket>(packet)->mGeometry[0]),
        selector = ea(&at<GeometryBank>(flushBase)->selected),
        flush = flushBase + (u32)(*at<be<u32>>(selector)) * u32(sizeof(BufferSlot)) + 4;
    for (u32 i = 0; i < 300; i++) {
        cloud_sdk::flushBuffer(flush, 0, (u32)(at<BufferSlot>(flush - 4)->size));
        flush += 2 * sizeof(BufferSlot);
    }
    *at<be<u32>>(selector) = (u32)(*at<be<u32>>(selector)) == 0;
    flushBase += sizeof(GeometryBank);
    selector = ea(&at<GeometryBank>(flushBase)->selected);
    flush = flushBase + (u32)(*at<be<u32>>(selector)) * u32(sizeof(BufferSlot)) + 4;
    for (u32 i = 0; i < 300; i++) {
        cloud_sdk::flushBuffer(flush, 0, (u32)(at<BufferSlot>(flush - 4)->size));
        flush += 2 * sizeof(BufferSlot);
    }
    *at<be<u32>>(selector) = (u32)(*at<be<u32>>(selector)) == 0;
    cloud_sdk::restoreRenderState(ea(&at<Renderer>(manager)->renderState));
}
} // namespace RainDraw
using namespace RainDraw;
/* WWHD glue (tools/verify/autoglue.py)
 */
