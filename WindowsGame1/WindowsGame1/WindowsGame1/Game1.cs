using System;
using System.Collections.Generic;
using System.Linq;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Audio;
using Microsoft.Xna.Framework.Content;
using Microsoft.Xna.Framework.GamerServices;
using Microsoft.Xna.Framework.Graphics;
using Microsoft.Xna.Framework.Input;
using Microsoft.Xna.Framework.Media;

namespace WindowsGame1
{
    /// <summary>
    /// This is the main type for your game
    /// </summary>
    public class Game1 : Microsoft.Xna.Framework.Game
    {
        GraphicsDeviceManager graphics;
        SpriteBatch spriteBatch;
        //DominoScene dominoScene;

        public Game1()
        {
            graphics = new GraphicsDeviceManager(this);
            this.IsMouseVisible = true;
            Content.RootDirectory = "Content";
            //dominoScene = new DominoScene();

            Console.WriteLine("PCPSolver:\n Team Members:\n Chris Dillard\n John Futch");


            // SIMPLE SOLVABLE INSTANCE
            // 
            //3 2
            //0 01 110
            //100 00 11
            //
            //

            PCPInstance instance = new PCPInstance();
            /*
            Pair p = new Pair();
            p.up= "0";
            p.down = "100";

            Pair p2 = new Pair();
            p2.up= "01";
            p2.down = "00";

            Pair p3 = new Pair();
            p3.up = "110";
            p3.down = "11";

            instance.arrPair = new Pair[3] { p, p2, p3 };
            */


            Pair p = new Pair();
            p.up = "100";
            p.down = "1";

            Pair p2 = new Pair();
            p2.up = "0";
            p2.down = "100";

            Pair p3 = new Pair();
            p3.up = "1";
            p3.down = "00";

            instance.arrPair = new Pair[3] { p, p2, p3 };

            instance.Print();

            PCPSolver.SolvePCPInstance(instance, 1);




        }

        /// <summary>
        /// Allows the game to perform any initialization it needs to before starting to run.
        /// This is where it can query for any required services and load any non-graphic
        /// related content.  Calling base.Initialize will enumerate through any components
        /// and initialize them as well.
        /// </summary>
        protected override void Initialize()
        {
            // TODO: Add your initialization logic here


            base.Initialize();
        }

        /// <summary>
        /// LoadContent will be called once per game and is the place to load
        /// all of your content.
        /// </summary>
        protected override void LoadContent()
        {
            // Create a new SpriteBatch, which can be used to draw textures.
            spriteBatch = new SpriteBatch(GraphicsDevice);
            //dominoScene.LoadContent(Content);

            // TODO: use this.Content to load your game content here
        }

        /// <summary>
        /// UnloadContent will be called once per game and is the place to unload
        /// all content.
        /// </summary>
        protected override void UnloadContent()
        {
            // TODO: Unload any non ContentManager content here
        }

        /// <summary>
        /// Allows the game to run logic such as updating the world,
        /// checking for collisions, gathering input, and playing audio.
        /// </summary>
        /// <param name="gameTime">Provides a snapshot of timing values.</param>
        protected override void Update(GameTime gameTime)
        {
            // Allows the game to exit
            if (GamePad.GetState(PlayerIndex.One).Buttons.Back == ButtonState.Pressed)
                this.Exit();

            //dominoScene.Update(gameTime);

            // TODO: Add your update logic here

            base.Update(gameTime);
        }

        /// <summary>
        /// This is called when the game should draw itself.
        /// </summary>
        /// <param name="gameTime">Provides a snapshot of timing values.</param>
        protected override void Draw(GameTime gameTime)
        {
            GraphicsDevice.Clear(Color.CornflowerBlue);

            // TODO: Add your drawing code here
            //dominoScene.Draw(spriteBatch);

            base.Draw(gameTime);
        }
    }
}
