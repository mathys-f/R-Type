---@diagnostic disable: missing-return

--------------------------------------------------------------------------------
--- ECS & Entities
--------------------------------------------------------------------------------

---@alias Tag string

--------------------------------------------------------------------------------
--- UI Components
--------------------------------------------------------------------------------

---@class UITransform
---@field x number # Horizontal position
---@field y number # Vertical position
---@field z number # Layer index
---@field w number # Width
---@field h number # Height
---@field anchor_x number # Horizontal anchor point (0.0 - 1.0)
---@field anchor_y number # Vertical anchor point (0.0 - 1.0)
---@field rotation number # Rotation in degrees

---@class UIStyle
---@field background_color Color
---@field background_color_hovered Color
---@field background_color_pressed Color
---@field text_color Color
---@field text_color_hovered Color
---@field text_color_pressed Color
---@field border_color Color
---@field border_color_hovered Color
---@field border_color_pressed Color
---@field border_radius number
---@field border_thickness number

---@class UINavigation # Used to defines controller/keyboard navigation
---@field up Tag
---@field down Tag
---@field right Tag
---@field left Tag

---@class UIButton
---@field on_click fun()

----------------------------------------------------------
-- Math & Utility Types
----------------------------------------------------------


---@class Color
---@field r integer
---@field g integer
---@field b integer
---@field a integer

----------------------------------------------------------
-- Engine API
----------------------------------------------------------

--- Creates a UI entity and returns its entity ID.
--- @param name string # Unique name (tag) for the entity.
function Create_ui_button(name) end

----------------------------------------------------------
-- UI Components Setters
----------------------------------------------------------

--- Sets the UITransform component for the given entity tag.
--- @param tag Tag
--- @param UITransform UITransform
function Set_ui_transform(tag, UITransform) end

--- Sets the UIStyle component for the given entity tag.
--- @param tag Tag
--- @param UIStyle UIStyle
function Set_ui_style(tag, UIStyle) end
