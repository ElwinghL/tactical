#include "entitycharacter.h"
#include "utility.h"

EntityCharacter::EntityCharacter(gf::ResourceManager* resMgr, const Character* characterPtr) :
    Entity{}, m_characterPtr{characterPtr}, m_sprite{resMgr->getTexture("placeholders/character.png")}
{
    assert(resMgr);
    assert(characterPtr);

    if (m_characterPtr->getTeam() == PlayerTeam::Cthulhu) {
        m_sprite.setScale({-1.0f, 1.0f}); // Mirrored
        m_sprite.setAnchor(gf::Anchor::CenterRight);
    } else {
        m_sprite.setColor(gf::Color::Red); // Red variant
        m_sprite.setAnchor(gf::Anchor::CenterLeft);
    }
    m_sprite.setOrigin(m_sprite.getOrigin() + gf::Vector2f{0.0f, -4.0f});
}

void EntityCharacter::update(gf::Time time)
{
    m_sprite.setPosition(gameToScreenPos(m_characterPtr->getPosition()));
}

void EntityCharacter::render(gf::RenderTarget& target, const gf::RenderStates& states)
{
    target.draw(m_sprite, states);
}
