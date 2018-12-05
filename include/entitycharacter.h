#ifndef ENTITYCHARACTER_H
#define ENTITYCHARACTER_H

#include "character.h"

#include <gf/Entity.h>
#include <gf/RenderStates.h>
#include <gf/RenderTarget.h>
#include <gf/ResourceManager.h>
#include <gf/Sprite.h>
#include <gf/Time.h>

#include <cassert>

// TODO EntityCharacter to EntityCharacters
class EntityCharacter : public gf::Entity {
public:
    EntityCharacter(gf::ResourceManager* resMgr, const Character* characterPtr);

    void update(gf::Time time) override;
    void render(gf::RenderTarget& target, const gf::RenderStates& states) override;

    const Character* getCharacterPtr() const
    {
        return m_characterPtr;
    }

private:
    const Character* m_characterPtr{nullptr};
    gf::Sprite m_sprite{};
};

#endif // ENTITYCHARACTER_H
