namespace FMIFramMakerApp
{
    partial class Form1
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.btnMaker = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // btnMaker
            // 
            this.btnMaker.Location = new System.Drawing.Point(473, 227);
            this.btnMaker.Name = "btnMaker";
            this.btnMaker.Size = new System.Drawing.Size(75, 23);
            this.btnMaker.TabIndex = 0;
            this.btnMaker.Text = "Maker";
            this.btnMaker.UseVisualStyleBackColor = true;
            this.btnMaker.Click += new System.EventHandler(this.btnMaker_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(571, 262);
            this.Controls.Add(this.btnMaker);
            this.Name = "Form1";
            this.Text = "Form ";
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button btnMaker;
    }
}

