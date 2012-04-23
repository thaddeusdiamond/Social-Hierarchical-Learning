package edu.shl.role_assignment;

import java.io.FileInputStream;
import java.util.Scanner;

/**
 * SkillTree describes a hierarchical tree with methods for
 * parsing from a standard file format and other utilities
 * 
 * @author Thaddeus Diamond <diamond@cs.yale.edu>
 **/
public class SkillTree {
    /** Each skill tree has a root node to begin traversal **/
    public SkillNode root;
    
    /**
     * Every node has a specific type, who can perform it
     **/
    public enum SkillType {
        /** An action can only be performed by humans **/       HUMAN,
        /** An action can only be performed by robots **/       ROBOT,
        /** Either a robot or human can perform an action **/   MIXED,
        /** This node was not yet given a label **/             UNLABELED
    }

    /**
     * We enumerate the possible states of our parser (a simple FSM)
     **/
    private enum ParserState {
        /** Next token will be a node id **/                    IDENTIFIER,
        /** Next token will be either a brace or type **/       CHILDREN_TYPE
    }
    
    /**
     * The default constructor for a SkillTree
     **/
    public SkillTree() {
        root = null;
    }
    
    /**
     * Single, root-node constructor for SkillTree w/identifier
     **/
    public SkillTree(String root_identifier) {
        root = new SkillNode(root_identifier, SkillType.UNLABELED, null);
    }
    
    /**
     * We provide a convenient constructor for reading in a file
     * and generating a skill tree from it.
     * <br/><br/>
     *  The standard file format used is JSON-like:
     * <br/><br/>
     * &nbsp;&nbsp;&lt;ROOT_ID&gt; {<br/>
     * &nbsp;&nbsp;&nbsp;&nbsp;&lt;NODE0_ID&gt; {<br/>
     * &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&lt;LEAF0_ID&gt; &lt;TYPE&gt;<br/>
     * &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&lt;LEAF1_ID&gt; &lt;TYPE&gt;<br/>
     * &nbsp;&nbsp;&nbsp;&nbsp;}<br/>
     * &nbsp;&nbsp;&nbsp;&nbsp;&lt;NODE1_ID&gt; {<br/>
     * &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&lt;LEAF2_ID&gt; &lt;TYPE&gt;<br/>
     * &nbsp;&nbsp;&nbsp;&nbsp;}<br/>
     * &nbsp;&nbsp;}
     * <br/><br/>
     *  Note that in this definition, any identifier cannot have whitespace
     *  in it.
     * <br/><br/>
     *  TYPE can be represented by the human-readable representation
     *  of {@link edu.shl.role_assignment.SkillTree.SkillType}
     *  
     *  @param    fileHandle    FileInputStream handle passed to Scanner
     **/
    public SkillTree(FileInputStream fileHandle) throws Exception {
        SkillNode current = root = null;
        Scanner file = new Scanner(fileHandle);
        
        // Parser FSM (initial state is the identifier for root node)
        ParserState state = ParserState.IDENTIFIER;
        while (file.hasNext()) {
            String input = file.next();
            switch (state) {
                case IDENTIFIER:
                    // Missing a node identifier before child list
                    if (input.equals("{")) {
                        throw new Exception("Missing node identifier");
                        
                    // First node in file
                    } else if (root == null) {
                        current = root = new SkillNode(input, SkillType.UNLABELED, null);
                    
                    // Finish with a set of children, move upward
                    } else if (input.equals("}")) {
                        current = current.parent();
                        continue;
                    
                    // New node ID, add to child list
                    } else {
                        current = current.addChild(input, SkillType.UNLABELED);
                    }
                    
                    state = ParserState.CHILDREN_TYPE;
                    break;
                    
                case CHILDREN_TYPE:
                    // { is beginning of child list, so do nothing
                    if (!input.equals("{")) {
                        // Switch based on human-readable type
                        if (input.toLowerCase().equals("human"))
                            current.setType(SkillType.HUMAN);
                        else if (input.toLowerCase().equals("robot"))
                            current.setType(SkillType.ROBOT);
                        else if (input.toLowerCase().equals("mixed"))
                            current.setType(SkillType.MIXED);
                        else
                            throw new Exception("Illegal SkillType specified");
                        
                        // Traverse back up to intermediate node from leaf
                        current = current.parent();
                    }
                    
                    state = ParserState.IDENTIFIER;
                    break;
            }
        }
    }
    
    /**
     * Given a unique ID, find the individual SkillNode object
     **/
    public SkillNode findSkillNodeByUUID(int uuid) {
        return findSkillNodeByUUID(uuid, root);
    }
    
    private SkillNode findSkillNodeByUUID(int uuid, SkillNode currentNode) {
        if (currentNode.uuid() == uuid)
            return currentNode;
        
        for (SkillNode child : currentNode.children()) {
            SkillNode result = findSkillNodeByUUID(uuid, child);
            if (result != null)
                return result;
        }
        return null;
    }
}