The Rooster 2D Graphics Engine is a fast, easy-to-use software system made for 
building sharp, colorful computer screens like gaming menus or radar dashboard 
displays. Instead of mixing all the code together, it uses a modular setup 
where every visual item (like a shape, a line, or a text box) acts like a 
block that you can plug in or snap together. This structure keeps the system 
organized and prevents common code bugs that slow down applications or cause them 
to crash.The engine also makes sure everything lines up perfectly down to the 
exact pixel. It has clever built-in tools that automatically center your text, 
rotate clock hands smoothly, and slice off any text or shapes that scroll past 
their container boxes so they do not clutter the screen. Whether you are building 
moving dial needles, a scrollable text notification feed, or custom layout squares, 
the Rooster Engine handles all the difficult background math automatically 
so you can focus on making your graphics look stunning.

Here is how each layer of your architectural chain breaks down from top to bottom:
The Application Window: Acts as the master desktop frame container defined by a single 
width and height resolution (e.g., 1280x720).
The Viewports: Divide that window's pixel estate into localized coordinate zones 
(like your 300x720 sidebar and 980x720 main canvas).
The UiObjects: Act as the organizational "nodes" or "chassis" attached to those 
viewports. Because they support a child tree structure (AddChild), a master 
UiObject (like the clock) can carry an infinite chain of nested sub-widgets.
The Components: Bolt right onto those UiObjects to give them behaviors. 
RotationComponents add automated spin logic, TextComponents add dynamic typography 
string buffers, and RenderComponents handle visibility mapping.
The Graphical Primitives: Sit inside the RenderComponents as raw, pure vector shapes 
(lines, circles, triangles, rectangles). They define the exact vertex meshes that are 
batched together and sent to your GPU shader pipelines to be painted onto your 
monitor screen.


<img width="410" height="311" alt="image" src="https://github.com/user-attachments/assets/07b861fd-b1b5-48eb-be7e-fbc5abfd5109" />
