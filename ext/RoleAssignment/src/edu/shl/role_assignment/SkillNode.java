package edu.shl.role_assignment;

import java.util.ArrayList;

import edu.shl.role_assignment.SkillTree.SkillType;

/**
 * Each node in the tree is a recursive structure with many children
 * (arbitrary m-way tree)
 **/
public class SkillNode {
    private int mUUID;
    private String mIdentifier;
    private SkillType mType;
    private ArrayList<SkillNode> mChildren;
    private SkillNode mParent;
    
    private static int mStaticCounter = 0;
    
    /**
     * The default Node constructor initializes an empty set of children
     * and takes an ID, type, and parent pointer
     * 
     * @param identifier        The string ID of this Node
     * @param type              What type this Node is
     * @param parent            Who the parent pointer of this Node is
     */
    public SkillNode(String identifier, SkillType type, SkillNode parent) {
        mUUID = mStaticCounter++;
        mIdentifier = identifier;
        mType = type;
        mParent = parent;
        mChildren = new ArrayList<SkillNode>();
    }
    
    /** @return The unique ID of the node **/
    public int uuid() {
        return mUUID;
    }
    
    /** @return Current plaintext identifier for specified Node **/
    public String identifier() {
        return mIdentifier;
    }
    
    /** @param  identifier      New String label to be assigned to Node **/
    public void setIdentifier(String identifier) {
        mIdentifier = identifier;
    }
    
    /** @return Current skill type of the Node **/
    public SkillType type() {
        return mType;
    }
    
    /** @param  type            The new type of the node to be set **/
    public void setType(SkillType type) {
        mType = type;
        for (SkillNode child : mChildren)
            child.setType(type);
    }
    
    /** @return  The parent pointer of the called Node object **/
    public SkillNode parent() {
        return mParent;
    }
    
    /** @return  An ArrayList object pointing to the child nodes **/
    public ArrayList<SkillNode> children() {
        return mChildren;
    }
    
    /** @return  A single Node represented at the specific index **/
    public SkillNode children(int i) {
        SkillNode child = null;
        try {
            child = mChildren.get(i);
        } catch (IndexOutOfBoundsException e) {
            // Do nothing...
        }
        return child;
    }
    
    /**
     * Disallow direct access to child nodes
     * 
     * @param   identifier      Child node string identifier
     * @param   type            Type (SkillType) of node being added
     * @return  Pointer to the child node added to tree
     **/
    public SkillNode addChild(String identifier, SkillType type) {
        SkillNode child = new SkillNode(identifier, type, this);
        mChildren.add(child);
        return child;
    }
    
    /**
     * Every tree can be labeled by traversing down and working upwards,
     * labeling mixed along the way back up instead of unlabeled
     **/
    public void assignLabel() throws Exception {
        // At a leaf
        if (mChildren.size() == 0) {
            if (mType == SkillType.UNLABELED)
                throw new Exception("Leaves of tree can't be unlabeled");
            return;
        }
        
        // Blindly assume nothing labeled in children
        boolean human_exists = false,
                robot_exists = false;
        for (SkillNode child : mChildren) {
            child.assignLabel();
            if (child.mType == SkillType.HUMAN || child.mType == SkillType.MIXED)
                human_exists = true;
            if (child.mType == SkillType.ROBOT || child.mType == SkillType.MIXED)
                robot_exists = true;
        }
        
        if (robot_exists && human_exists)
            mType = SkillType.MIXED;
        else if (robot_exists)
            mType = SkillType.ROBOT;
        else if (human_exists)
            mType = SkillType.HUMAN;
        else
            throw new Exception("There was an error labeling tree");
    }
}