#include "CSprite.h"
#include "CImage.h"
#include "../scbwdata.h"
#include <algorithm>
#include <cassert>

//Functionally identical to playSpriteIscript() (offset 0x00499D00)
void CSprite::playIscriptAnim(IscriptAnimation::Enum animation) {
  assert(this);
  for (CImage *img = this->imageHead; img; img = img->link.next)
    img->playIscriptAnim(animation);
}

void CSprite::free() {
  assert(this);
  CImage *image = this->imageHead;
  while (image) {
    CImage *nextImage = image->link.next;
    image->free();
    image = nextImage;
  }

  const int y = CLAMP(this->position.y / 32, 0, mapTileSize->height - 1);
  const CListExtern<CSprite, &CSprite::link>
    spritesOnTileRow(spritesOnTileRow->heads[y],
                     spritesOnTileRow->tails[y]);

  spritesOnTileRow.unlink(this);
  unusedSprites.insertAfterHead(this);
}

void CSprite::setPosition(u16 x, u16 y) {
  assert(this);
  if (this->position.x == x && this->position.y == y) return;

  const int oldTileY = CLAMP(this->position.y / 32, 0, mapTileSize->height - 1);
  const int newTileY = CLAMP(y / 32, 0, mapTileSize->height - 1);

  this->position.x = x;
  this->position.y = y;

  if (oldTileY != newTileY) {
    const CListExtern<CSprite, &CSprite::link>
      spritesOnOldTileRow(spritesOnTileRow->heads[oldTileY],
                          spritesOnTileRow->tails[oldTileY]),
      spritesOnNewTileRow(spritesOnTileRow->heads[newTileY],
                          spritesOnTileRow->tails[newTileY]);

    spritesOnOldTileRow.unlink(this);
    spritesOnNewTileRow.insertAfterHead(this);
  }

  for (CImage *i = this->imageHead; i; i = i->link.next)
    i->flags |= 1;
}


//-------- Create overlay --------//

void createUpdateImageSomething(CImage *image) {
  const u32 Func_createUpdateImageSomething = 0x004D66B0;

  __asm {
    PUSHAD
    MOV EAX, image
    CALL Func_createUpdateImageSomething
    POPAD
  }
}

void updateImageDirection(CImage *image, u32 direction) {
  const u32 Func_updateImageDirection = 0x004D5EA0;

  __asm {
    PUSHAD
    PUSH direction
    MOV EAX, image
    CALL Func_updateImageDirection
    POPAD
  }
}

//-------- Create overlay --------//

//Identical to function @ 0x00498E00
CImage* CSprite::createOverlay(u32 imageId, s8 x, s8 y, u32 direction) {
  assert(this);
  CImage *overlay = unusedImages.popHead();
  
  if (overlay) {
    const CListExtern<CImage, &CImage::link> images(this->imageHead, this->imageTail);
    if (this->imageHead) {
      images.insertBefore(overlay, this->mainGraphic);
    }
    else {
      this->mainGraphic = overlay;
      images.insertAfterHead(overlay);
    }

    overlay->initializeData(this, imageId, x, y);
    createUpdateImageSomething(overlay);
    updateImageDirection(overlay, direction);
  }
  return overlay;
}

//Identical to function @ 0x00498EA0
CImage* CSprite::createTopOverlay(u32 imageId, s8 x, s8 y, u32 direction) {
  assert(this);
  CImage *overlay = unusedImages.popHead();
  
  if (overlay) {
    const CListExtern<CImage, &CImage::link> images(this->imageHead, this->imageTail);
    if (this->imageHead) {
      images.insertBeforeHead(overlay);
    }
    else {
      this->mainGraphic = overlay;
      images.insertAfterHead(overlay);
    }

    overlay->initializeData(this, imageId, x, y);
    createUpdateImageSomething(overlay);
    updateImageDirection(overlay, direction);
  }
  return overlay;
}


//-------- Remove overlay --------//

//Based on function @ 0x004E5CF0
void CSprite::removeOverlay(u32 imageIdStart, u32 imageIdEnd) {
  assert(this);
  assert(imageIdStart <= imageIdEnd);

  for (CImage *image = this->imageHead; image; image = image->link.next) {
    if (imageIdStart <= image->id && image->id <= imageIdEnd) {
      image->free();
      return;
    }
  }
}

void CSprite::removeOverlay(u32 imageId) {
  assert(this);

  for (CImage *image = this->imageHead; image; image = image->link.next) {
    if (image->id == imageId) {
      image->free();
      return;
    }
  }
}

//-------- Get overlay --------//

//Identical to function @ 0x00497460
CImage* CSprite::getOverlay(u16 imageId) const {
  assert(this);

  for (CImage *image = this->imageHead; image; image = image->link.next)
    if (image->id == imageId)
      return image;

  return NULL;
}

//-------- Other --------//

bool CSprite::isVisibleTo(s8 playerId) const {
  assert(this);
  return (this->visibilityFlags & (1 << playerId)) != 0;
}

