/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_MISC_APRIL_11_2016)
#define ELEMENTS_MISC_APRIL_11_2016

#include <elements/element/element.hpp>
#include <elements/element/proxy.hpp>
#include <elements/element/text.hpp>
#include <elements/support/theme.hpp>
#include <elements/support/font.hpp>
#include <functional>
#include <string>
#include <string_view>

namespace cycfi { namespace elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Box: A simple colored box.
   ////////////////////////////////////////////////////////////////////////////
   struct box_element : element
   {
      box_element(color color_)
       : _color(color_)
      {}

      void draw(context const& ctx) override
      {
         auto& cnv = ctx.canvas;
         cnv.fill_style(_color);
         cnv.fill_rect(ctx.bounds);
      }

      color _color;
   };

   inline auto box(color color_)
   {
      return box_element{ color_ };
   }

   ////////////////////////////////////////////////////////////////////////////
   // RBox: A simple colored rounded-box.
   ////////////////////////////////////////////////////////////////////////////
   struct rbox_element : element
   {
      rbox_element(color color_, float radius = 4)
       : _color(color_)
       , _radius(radius)
      {}

      void draw(context const& ctx) override
      {
         auto& cnv = ctx.canvas;
         cnv.begin_path();
         cnv.round_rect(ctx.bounds, _radius);
         cnv.fill_style(_color);
         cnv.fill();
      }

      color _color;
      float _radius;
   };

   inline auto rbox(color color_, float radius = 4)
   {
      return rbox_element{ color_, radius };
   }

   ////////////////////////////////////////////////////////////////////////////
   // Basic Element
   //
   // The basic element takes in a function that draws something
   ////////////////////////////////////////////////////////////////////////////
   template <typename F>
   class basic_element : public element
   {
   public:

      basic_element(F f)
       : f(f)
      {}

      void
      draw(context const& ctx) override
      {
         f(ctx);
      }

   private:

      F f;
   };

   template <typename F>
   inline basic_element<F> basic(F f)
   {
      return { f };
   }

   ////////////////////////////////////////////////////////////////////////////
   // Panels
   ////////////////////////////////////////////////////////////////////////////
   class panel : public element
   {
   public:

                     panel(float opacity_ = get_theme().panel_color.alpha)
                      : _opacity(opacity_)
                     {}

      void           draw(context const& ctx) override;

   private:

      float          _opacity;
   };

   ////////////////////////////////////////////////////////////////////////////
   // Frames
   ////////////////////////////////////////////////////////////////////////////
   struct frame : public element
   {
      void           draw(context const& ctx) override;
   };

   ////////////////////////////////////////////////////////////////////////////
   // Headings
   ////////////////////////////////////////////////////////////////////////////
   class heading : public element, public text_base
   {
      public:
                              heading(
                                 std::string text
                               , float size_ = 1.0
                              );

                              heading(
                                 std::string text
                               , font font_
                               , float size = 1.0
                              );

      view_limits             limits(basic_context const& ctx) const override;
      void                    draw(context const& ctx) override;

      std::string const&      text() const override                  { return _text; }
      char const*             c_str() const override                 { return _text.c_str(); }
      void                    text(std::string_view text) override   { _text = text; }

      elements::font const&   font() const                           { return _font; }
      void                    font(elements::font_descr descr)       { _font = elements::font(descr); }

      float                   size() const                           { return _size; }
      void                    size(float size_)                      { _size = size_; }

      using element::text;

   private:

      std::string             _text;
      elements::font          _font;
      float                   _size;
   };

   ////////////////////////////////////////////////////////////////////////////
   // Title Bars
   ////////////////////////////////////////////////////////////////////////////
   class title_bar : public element
   {
   public:

      void                    draw(context const& ctx) override;
   };

   ////////////////////////////////////////////////////////////////////////////
   // Labels
   ////////////////////////////////////////////////////////////////////////////
   class label : public element, public text_base
   {
   public:
                              label(
                                 std::string text
                               , float size = 1.0
                              );

                              label(
                                 std::string text
                               , font font_
                               , float size = 1.0
                              );

      view_limits             limits(basic_context const& ctx) const override;
      void                    draw(context const& ctx) override;

      std::string const&      text() const override                  { return _text; }
      char const*             c_str() const override                 { return _text.c_str(); }
      void                    text(std::string_view text) override   { _text = text; }

      elements::font const&   font() const                           { return _font; }
      void                    font(elements::font_descr descr)       { _font = elements::font(descr); }
      void                    font(elements::font font_)             { _font = font_; }

      float                   size() const                           { return _size; }
      void                    size(float size_)                      { _size = size_; }

      using element::text;

   private:

      std::string             _text;
      elements::font          _font;
      float                   _size;
   };

   ////////////////////////////////////////////////////////////////////////////
   // Grid Lines
   ////////////////////////////////////////////////////////////////////////////
   class vgrid_lines : public element
   {
   public:

                              vgrid_lines(float major_divisions, float minor_divisions)
                               : _major_divisions(major_divisions)
                               , _minor_divisions(minor_divisions)
                              {}

      void                    draw(context const& ctx) override;

   private:

      float                   _major_divisions;
      float                   _minor_divisions;
   };

   ////////////////////////////////////////////////////////////////////////////
   // Icons
   ////////////////////////////////////////////////////////////////////////////
   struct icon : element
   {
                              icon(std::uint32_t code_, float size_ = 1.0);

      view_limits             limits(basic_context const& ctx) const override;
      void                    draw(context const& ctx) override;

      std::uint32_t           _code;
      float                   _size;
   };

   ////////////////////////////////////////////////////////////////////////////
   // Key Intercept
   ////////////////////////////////////////////////////////////////////////////
   template <typename Subject>
   struct key_intercept_element : public proxy<Subject>
   {
      using base_type = proxy<Subject>;

                              key_intercept_element(Subject&& subject)
                               : base_type(std::forward<Subject>(subject))
                              {}

      bool                    key(context const& ctx, key_info k) override;
      bool                    is_control() const override { return true; }
      bool                    wants_focus() const override { return true; }

      using key_function = std::function<bool(key_info k)>;

      key_function            on_key = [](auto){ return false; };
   };

   template <typename Subject>
   inline key_intercept_element<Subject>
   key_intercept(Subject&& subject)
   {
      return { std::forward<Subject>(subject) };
   }

   template <typename Subject>
   inline bool key_intercept_element<Subject>::key(context const& ctx, key_info k)
   {
      if (on_key(k))
         return true;
      return this->subject().key(ctx, k);
   }
}}

#endif
