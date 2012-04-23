package edu.shl.role_assignment;

import java.io.FileOutputStream;

import android.app.Activity;
import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Path;
import android.os.Bundle;
import android.util.Log;
import android.view.ContextMenu;
import android.view.ContextMenu.ContextMenuInfo;
import android.view.MenuItem;
import android.view.View;
import android.widget.RelativeLayout;
import android.widget.RelativeLayout.LayoutParams;
import android.widget.TextView;
import edu.shl.role_assignment.SkillTree.SkillType;

/**
 * Main activity for showing our role assignments
 * 
 * @author tcd2
 */
public class RoleAssignmentActivity extends Activity {
    /** Drawing constants **/
    private final int NODE_WIDTH = 150;
    private final int NODE_HEIGHT = 40;
    private final double SCALE_HEIGHT = 2;
    private final double SCALE_WIDTH = 2.5;
    private final int WINDOW_PADDING = 10;
    private final int PAINT_STROKE_WIDTH = 7;
    
    /** Maintain the main screen layout and all node views **/
    private int mWindowHeight;
    private int mWindowWidth;
    private RelativeLayout mScreenLayout;
    
    /** This activity can only view one skill tree at a time **/
    private SkillTree mSkillTree;
    
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        // Initialize main view
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        
        // Set screen-dependent items
        mWindowWidth  = getWindowManager().getDefaultDisplay().getWidth();
        mWindowHeight = getWindowManager().getDefaultDisplay().getHeight();
        mScreenLayout = (RelativeLayout) findViewById(R.id.main_layout);
        mScreenLayout.setPadding(WINDOW_PADDING, WINDOW_PADDING,
                                 WINDOW_PADDING, WINDOW_PADDING);
        
        // Create a dummy tree
        String test_file = "test_file.tree";
        String tree_str
            = "  <ROOT_ID> {\n    <NODE0_ID> {\n      "
                + "<LEAF0_ID> HUMAN\n<LEAF1_ID> MIXED\n"
                + "    }\n    <NODE1_ID> {\n      <LEAF2_ID> ROBOT\n    }\n  }";
        
        // Write then read the tree
        try {
            FileOutputStream fos = openFileOutput(test_file, Context.MODE_PRIVATE);
            fos.write(tree_str.getBytes());
            fos.close();
            
            mSkillTree = new SkillTree(openFileInput(test_file));
        
        // I/O error and parsing error handling
        } catch (Exception e) {
            Log.e("SkillTree", "Error with tree I/O");
            e.printStackTrace();
        }
        
        // Update the view
        refreshView();
    }
    
    /**
     * We keep a way of instantly refreshing the entire viewport
     **/
    private void refreshView() {
        try {
            if (mSkillTree != null)
                mSkillTree.root.assignLabel();
            removeNodeViews();
            drawTree(mSkillTree.root, 0, mWindowWidth / 2, null);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
    
    /**
     * Remove all nodes from the view except the legend
     */
    private void removeNodeViews() {
        for (int i = 0; i < mScreenLayout.getChildCount(); i++) {
            View child = mScreenLayout.getChildAt(i);
            if (child.getId() == R.id.legend)
                continue;
            mScreenLayout.removeView(child);
        }
    }
    
    /**
     * Be capable of drawing a SkillTree
     * 
     * @param   root        The root node this tree starts at
     * @param   depth       Depth of the current root node
     * @param   center      Pixel around which subtree is centered
     * @param   parentView  The parent View object of this current sub-branch
     **/
    private void drawTree(SkillNode root, int depth, int center, View parentView) {
        int top = (int) (depth * NODE_HEIGHT * SCALE_HEIGHT);
        View newNode = drawNode(root, top, center, parentView);
        
        for (int i = 0; i < root.children().size(); i++) {
            drawTree(root.children(i),
                     depth + 1,
                     (int) (center
                             - ((root.children().size() - 1) / 2.0 - i)
                               * NODE_WIDTH * SCALE_WIDTH / (depth + 1)),
                     newNode);
        }
    }
    
    /**
     * Draw the individual node component to screen
     * 
     * @param   node        The node whose info is being drawn
     * @param   top         Top of current node
     * @param   center      Pixel around which node is centered
     * @param   parentView  View representing parent node of current node
     * @return  Reference to the newly created node view
     **/
    private View drawNode(SkillNode node, int top, int center, View parentView) {
        View newNode = getLayoutInflater().inflate(R.layout.node, null);
        RelativeLayout.LayoutParams layout
            = new RelativeLayout.LayoutParams(NODE_WIDTH, NODE_HEIGHT);
        layout.topMargin = top;
        layout.leftMargin = center - (NODE_WIDTH / 2);
        
        // Fill in the blanks
        newNode.setId(node.uuid());
        ((TextView) newNode.findViewById(R.id.node_identifier)).setText(
                node.identifier());
        newNode.setBackgroundColor(nodeColor(node));

        // Make a line connecting to the parent node
        if (parentView != null) {
            RelativeLayout.LayoutParams pParams
                = (LayoutParams) parentView.getLayoutParams();
            int pCenter = pParams.leftMargin + NODE_WIDTH / 2;
            int pTop  = pParams.topMargin;
                
            View line =  new Line(this, center, top, pCenter, pTop);
            mScreenLayout.addView(line);
            parentView.bringToFront();
        }
        
        // Hook up on click and add the view
        registerForContextMenu(newNode);
        mScreenLayout.addView(newNode, layout);
        return newNode;
    }
    
    /**
     * Retrieve the color of a node based on its SkillType
     * 
     * @param   node        The node whose color is being retrieved
     * @return  An int (hex value) representing RGB color of node
     **/
    private int nodeColor(SkillNode node) {
        switch (node.type()) {
            case HUMAN:
                return Color.GREEN;
            case ROBOT:
                return Color.BLUE;
            case MIXED:
                return Color.YELLOW;
            case UNLABELED:
                return Color.GRAY;
        }
        return -1;
    }
    
    /**
     * Since nodes register for ContextMenus our Activity needs to create
     * that functionality.
     **/
    @Override
    public void onCreateContextMenu(ContextMenu menu, View v, ContextMenuInfo info) {
        super.onCreateContextMenu(menu, v, info);
        menu.setHeaderTitle("Select Skill Type for Node and Children");
        menu.add(0, v.getId(), 0, "Human");
        menu.add(0, v.getId(), 1, "Robot");
        menu.add(0, v.getId(), 2, "Mixed");
    }
    
    /**
     * When the ContextMenu is selected for a view, we find that node and
     * update it's type, then refresh the view
     **/
    @Override
    public boolean onContextItemSelected(MenuItem item) {
        SkillType updatedType = null;
        if (item.getTitle().equals("Human"))
            updatedType = SkillType.HUMAN;
        else if (item.getTitle().equals("Robot"))
            updatedType = SkillType.ROBOT;
        else if (item.getTitle().equals("Mixed"))
            updatedType = SkillType.MIXED;
        
        mSkillTree.findSkillNodeByUUID(item.getItemId()).setType(updatedType);
        refreshView();
        return true;
    }
    
    /**
     * In order to draw simple lines Android makes us extend a custom view
     * to be placed in our drawing Canvas.
     **/
    private class Line extends View {
        private Path mPath;
        private Paint mPaint;

        /**
         * The constructor for a Line connects two points in X-Y space
         * 
         * @param context       Context view is being made in
         * @param x1            X-Coordinate of first point
         * @param y1            Y-Coordinate of first point
         * @param x2            X-Coordinate of second point
         * @param y2            Y-Coordinate of second point
         */
        public Line(Context context, int x1, int y1, int x2, int y2) {
            super(context);

            mPath = new Path();
            mPath.moveTo(x1, y1);
            mPath.lineTo(x2, y2);
            
            mPaint = new Paint();
            mPaint.setColor(0xFFFFFFFF);
            mPaint.setStyle(Paint.Style.STROKE);
            mPaint.setStrokeWidth(PAINT_STROKE_WIDTH);
        }

        @Override
        protected void onDraw(Canvas canvas) {
            canvas.drawPath(mPath, mPaint);
        }
    }
}